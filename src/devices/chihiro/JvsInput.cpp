#include "JvsInput.h"
#include <Windows.h>
#include <XInput.h>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <map>
#include <cctype>
#include <cstdlib>
#include <cstring>

extern HWND g_hEmuWindow;
namespace JvsInput {
namespace {
std::string profile;
bool enabled = false;
jvs_input_states_t state;
bool test = false, service = false, mouseActive = false;
float mouseX = 0, mouseY = 0;
std::mutex boundsMutex;
float bounds[4] = {0, 0, 1, 1};
XINPUT_STATE pads[2] = {};
bool connected[2] = {};
ULONGLONG nextScan[2] = {};
using GetPadState = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
GetPadState getPadState = nullptr;
std::map<std::string, std::string> settings;
ULONGLONG nextReload = 0;

void Reload() {
 settings.clear();
 for (int i = 0; i < 12; ++i) {
  const std::string section = i == 0 ? "System" : i == 1 ? "General" :
   i < 4 ? "Player" + std::to_string(i - 1) : "Analog" + std::to_string(i - 3);
  char buffer[4096] = {};
  GetPrivateProfileSectionA(section.c_str(), buffer, sizeof(buffer), profile.c_str());
  for (const char* entry = buffer; *entry; entry += strlen(entry) + 1) {
   const std::string line(entry);
   const auto equals = line.find('=');
   if (equals != std::string::npos) settings[section + "/" + line.substr(0, equals)] = line.substr(equals + 1);
  }
 }
}

std::string Read(const std::string& section, const char* key, const char* fallback = "") {
 const auto found = settings.find(section + "/" + key);
 return found == settings.end() ? fallback : found->second;
}
int Key(const std::string& name) {
 if (name.size() == 1) return toupper((unsigned char)name[0]);
 if (name == "Left") return VK_LEFT;
 if (name == "Right") return VK_RIGHT;
 if (name == "Up") return VK_UP;
 if (name == "Down") return VK_DOWN;
 if (name == "Space") return VK_SPACE;
 if (name == "Enter") return VK_RETURN;
 if (name == "F1") return VK_F1;
 if (name == "F2") return VK_F2;
 return 0;
}
bool Pressed(const std::string& binding, int player) {
 std::istringstream entries(binding);
 std::string token;
 while (std::getline(entries, token, ',')) {
  token.erase(std::remove_if(token.begin(), token.end(), [](unsigned char c) { return isspace(c); }), token.end());
  if (token.compare(0, 4, "Key.") == 0) {
   const int key = Key(token.substr(4));
   if (key && (GetAsyncKeyState(key) & 0x8000)) return true;
  } else if (token.compare(0, 6, "Mouse.") == 0 && mouseActive) {
   const int key = token == "Mouse.Left" ? VK_LBUTTON : token == "Mouse.Right" ? VK_RBUTTON : 0;
   if (key && (GetAsyncKeyState(key) & 0x8000)) return true;
  } else if (token.compare(0, 4, "Pad.") == 0 && connected[player]) {
   const auto& pad = pads[player].Gamepad;
   const std::string name = token.substr(4);
   WORD mask = name == "A" ? XINPUT_GAMEPAD_A : name == "B" ? XINPUT_GAMEPAD_B :
    name == "X" ? XINPUT_GAMEPAD_X : name == "Y" ? XINPUT_GAMEPAD_Y :
    name == "Start" ? XINPUT_GAMEPAD_START : name == "Back" ? XINPUT_GAMEPAD_BACK :
    name == "Up" ? XINPUT_GAMEPAD_DPAD_UP : name == "Down" ? XINPUT_GAMEPAD_DPAD_DOWN :
    name == "Left" ? XINPUT_GAMEPAD_DPAD_LEFT : name == "Right" ? XINPUT_GAMEPAD_DPAD_RIGHT : 0;
   if ((pad.wButtons & mask) || (name == "RT" && pad.bRightTrigger > 100) || (name == "LT" && pad.bLeftTrigger > 100)) return true;
  }
 }
 return false;
}
float Clamp(float value) { return (std::max)(-1.0f, (std::min)(value, 1.0f)); }
void Mouse() {
 mouseActive = false;
 mouseX = mouseY = 0;
 const HWND foreground = GetForegroundWindow();
 if (!g_hEmuWindow || !foreground || IsIconic(g_hEmuWindow) ||
  (foreground != g_hEmuWindow && !IsChild(foreground, g_hEmuWindow))) return;
 RECT client;
 POINT point;
 if (!GetClientRect(g_hEmuWindow, &client) || !GetCursorPos(&point) || !ScreenToClient(g_hEmuWindow, &point)) return;
 float left, top, width, height;
 {
  std::lock_guard<std::mutex> lock(boundsMutex);
  left = bounds[0] * client.right; top = bounds[1] * client.bottom;
  width = (bounds[2] - bounds[0]) * client.right;
  height = (bounds[3] - bounds[1]) * client.bottom;
 }
 const float aspect = (float)atof(Read("General", "MouseAspectRatio", "0").c_str());
 if (aspect > 0 && std::isfinite(aspect) && client.bottom > 0) {
  width = (float)client.right; height = (float)client.bottom;
  left = top = 0;
  if (width / height > aspect) { width = height * aspect; left = (client.right - width) / 2; }
  else { height = width / aspect; top = (client.bottom - height) / 2; }
 }
 if (width <= 1 || height <= 1) return;
 mouseActive = point.x >= left && point.x < left + width && point.y >= top && point.y < top + height;
 mouseX = Clamp(2 * (point.x - left) / (width - 1) - 1);
 mouseY = Clamp(2 * (point.y - top) / (height - 1) - 1);
}
uint16_t Axis(const std::string& section) {
 const int player = (std::max)(0, (std::min)(1, atoi(Read(section, "Pad", "1").c_str()) - 1));
 const std::string source = Read(section, "Source");
 float value = 0;
 if (source == "MouseX") value = mouseX;
 else if (source == "MouseY") value = mouseY;
 else if (connected[player]) {
  const auto& pad = pads[player].Gamepad;
  if (source == "LStickX") value = Clamp(pad.sThumbLX / 32767.0f);
  if (source == "LStickY") value = Clamp(pad.sThumbLY / 32767.0f);
  if (source == "RStickX") value = Clamp(pad.sThumbRX / 32767.0f);
  if (source == "RStickY") value = Clamp(pad.sThumbRY / 32767.0f);
  if (source.find("Stick") != std::string::npos) {
   const float deadzone = (std::max)(0.0f, (std::min)(0.99f, (float)atof(Read(section, "Deadzone", "0.1").c_str())));
   value = std::fabs(value) <= deadzone ? 0 : std::copysign((std::fabs(value) - deadzone) / (1 - deadzone), value);
  }
 }
 const float deflection = (std::max)(0.0f, (std::min)(1.0f, (float)atof(Read(section, "KeyDeflection", "1").c_str())));
 if (Pressed(Read(section, "KeyMin"), player)) value -= deflection;
 if (Pressed(Read(section, "KeyMax"), player)) value += deflection;
 value = Clamp(value);
 if (Read(section, "Invert", "0") == "1") value = -value;
 const long result = 32768 + std::lround(value * 32768);
 return (uint16_t)(std::max)(0L, (std::min)(65535L, result));
}
}
void SetRenderBounds(float left, float top, float right, float bottom) {
 std::lock_guard<std::mutex> lock(boundsMutex);
 bounds[0] = left; bounds[1] = top; bounds[2] = right; bounds[3] = bottom;
}
void Init(const std::string& dataPath, const std::string& executable) {
 std::string name = std::filesystem::path(executable).stem().string();
 std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return (char)tolower(c); });
 enabled = name.find("hod3") != std::string::npos;
 if (!enabled) return;
 profile = (std::filesystem::path(dataPath) / ("chihiro_input_" + name + ".ini")).string();
 if (!std::filesystem::exists(profile)) {
  std::ofstream out(profile);
  out << "; Live input profile. Pad refers to the player's XInput controller.\n[General]\nRequireFocus=1\nMouseAspectRatio=0\n\n[System]\nCoin1=Key.5,Pad.Back\nCoin2=Key.6,Pad.Back\nTest=Key.F1\nService=Key.F2\n";
  for (int p = 0; p < 2; ++p) {
   out << "\n[Player" << p + 1 << "]\nStart=Key." << p + 1 << ",Pad.Start\nService=Key.F2\nUp=Key.Up,Pad.Up\nDown=Key.Down,Pad.Down\nLeft=Key.Left,Pad.Left\nRight=Key.Right,Pad.Right\n";
   out << "Button1=" << (p == 0 ? "Mouse.Left," : "") << (p == 0 ? "Key.A" : "Key.J") << ",Pad.A,Pad.RT\nButton2=" << (p == 0 ? "Mouse.Right," : "") << (p == 0 ? "Key.S" : "Key.K") << ",Pad.B\nButton3=" << (p == 0 ? "Key.D" : "Key.L") << ",Pad.X\nButton4=" << (p == 0 ? "Key.F" : "Key.P") << ",Pad.Y\n";
  }
  for (int a = 0; a < 8; ++a) {
   out << "\n[Analog" << a + 1 << "]\nSource=";
   if (a < 4) out << (a < 2 ? (a % 2 ? "MouseY" : "MouseX") : (a % 2 ? "LStickY" : "LStickX"));
   out << "\nPad=" << (a >= 2 ? 2 : 1) << "\nInvert=" << (a == 3 ? 1 : 0) << "\nDeadzone=0.1\nKeyMin=\nKeyMax=\nKeyDeflection=1\n";
  }
 }
 Reload();
 for (const char* dll : {"xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll"}) {
  HMODULE module = LoadLibraryA(dll);
  if (module) { getPadState = (GetPadState)GetProcAddress(module, "XInputGetState"); break; }
 }
}
void Poll() {
 if (!enabled) return;
 const ULONGLONG now = GetTickCount64();
 if (now >= nextReload) { Reload(); nextReload = now + 1000; }
 const HWND foreground = GetForegroundWindow();
 const bool focused = g_hEmuWindow && foreground &&
  (foreground == g_hEmuWindow || GetAncestor(g_hEmuWindow, GA_ROOT) == foreground);
 if (Read("General", "RequireFocus", "1") != "0" && !focused) {
  state = {}; test = service = false;
  return;
 }
 for (int p = 0; p < 2; ++p) {
  if (!getPadState || (!connected[p] && now < nextScan[p])) continue;
  connected[p] = getPadState(p, &pads[p]) == ERROR_SUCCESS;
  if (!connected[p]) { pads[p] = {}; nextScan[p] = now + 1000; }
 }
 Mouse();
 test = Pressed(Read("System", "Test"), 0);
 service = Pressed(Read("System", "Service"), 0);
 state.switches.system.test = test;
 for (int p = 0; p < 2; ++p) {
  const std::string section = "Player" + std::to_string(p + 1);
  auto& player = state.switches.player[p];
  player.start = Pressed(Read(section, "Start"), p);
  player.service = Pressed(Read(section, "Service"), p);
  player.up = Pressed(Read(section, "Up"), p); player.down = Pressed(Read(section, "Down"), p);
  player.left = Pressed(Read(section, "Left"), p); player.right = Pressed(Read(section, "Right"), p);
  for (int b = 0; b < 10; ++b) player.button[b] = Pressed(Read(section, ("Button" + std::to_string(b + 1)).c_str()), p);
  static bool previous[2] = {};
  const bool coin = Pressed(Read("System", ("Coin" + std::to_string(p + 1)).c_str()), p);
  state.coins[p].coins = coin && !previous[p] ? 1 : 0;
  previous[p] = coin;
 }
 for (int a = 0; a < 8; ++a) state.analog[a].value = Axis("Analog" + std::to_string(a + 1));
}
const jvs_input_states_t& GetState() { return state; }
bool IsEnabled() { return enabled; }
bool Test() { return test; }
bool Service() { return service; }
}
