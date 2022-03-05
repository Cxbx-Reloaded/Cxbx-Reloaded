// Adapted from component by mav.northwind
// See https://www.codeproject.com/Articles/9196/Links-with-arbitrary-text-in-a-RichTextBox

using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace CxbxDebugger
{
    public class RicherTextBox : RichTextBox
    {
        #region Interop-Defines

        [StructLayout(LayoutKind.Sequential)]
        private struct CHARFORMAT2_STRUCT
        {
            public uint cbSize;
            public uint dwMask;
            public uint dwEffects;
            public int yHeight;
            public int yOffset;
            public int crTextColor;
            public byte bCharSet;
            public byte bPitchAndFamily;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            public char[] szFaceName;
            public ushort wWeight;
            public ushort sSpacing;
            public int crBackColor; // Color.ToArgb() -> int
            public int lcid;
            public int dwReserved;
            public short sStyle;
            public short wKerning;
            public byte bUnderlineType;
            public byte bAnimation;
            public byte bRevAuthor;
            public byte bReserved1;
        }

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr SendMessage(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam);

        private const int WM_SETREDRAW = 0x0b;
        private const int WM_USER = 0x0400;
        private const int EM_GETCHARFORMAT = WM_USER + 58;
        private const int EM_SETCHARFORMAT = WM_USER + 68;
        private const int EM_SETEVENTMASK = WM_USER + 69;

        private const int SCF_SELECTION = 0x0001;
        private const int SCF_WORD = 0x0002;
        private const int SCF_ALL = 0x0004;

        private IntPtr OldEventMask;

        #region CHARFORMAT2 Flags
        private const uint CFE_BOLD = 0x0001;
        private const uint CFE_ITALIC = 0x0002;
        private const uint CFE_UNDERLINE = 0x0004;
        private const uint CFE_STRIKEOUT = 0x0008;
        private const uint CFE_PROTECTED = 0x0010;
        private const uint CFE_LINK = 0x0020;
        private const uint CFE_AUTOCOLOR = 0x40000000;
        private const uint CFE_SUBSCRIPT = 0x00010000;        /* Superscript and subscript are */
        private const uint CFE_SUPERSCRIPT = 0x00020000;      /*  mutually exclusive			 */

        private const int CFM_SMALLCAPS = 0x0040;           /* (*)	*/
        private const int CFM_ALLCAPS = 0x0080;         /* Displayed by 3.0	*/
        private const int CFM_HIDDEN = 0x0100;          /* Hidden by 3.0 */
        private const int CFM_OUTLINE = 0x0200;         /* (*)	*/
        private const int CFM_SHADOW = 0x0400;          /* (*)	*/
        private const int CFM_EMBOSS = 0x0800;          /* (*)	*/
        private const int CFM_IMPRINT = 0x1000;         /* (*)	*/
        private const int CFM_DISABLED = 0x2000;
        private const int CFM_REVISED = 0x4000;

        private const int CFM_BACKCOLOR = 0x04000000;
        private const int CFM_LCID = 0x02000000;
        private const int CFM_UNDERLINETYPE = 0x00800000;       /* Many displayed by 3.0 */
        private const int CFM_WEIGHT = 0x00400000;
        private const int CFM_SPACING = 0x00200000;     /* Displayed by 3.0	*/
        private const int CFM_KERNING = 0x00100000;     /* (*)	*/
        private const int CFM_STYLE = 0x00080000;       /* (*)	*/
        private const int CFM_ANIMATION = 0x00040000;       /* (*)	*/
        private const int CFM_REVAUTHOR = 0x00008000;

        private const uint CFM_BOLD = 0x00000001;
        private const uint CFM_ITALIC = 0x00000002;
        private const uint CFM_UNDERLINE = 0x00000004;
        private const uint CFM_STRIKEOUT = 0x00000008;
        private const uint CFM_PROTECTED = 0x00000010;
        private const uint CFM_LINK = 0x00000020;
        private const uint CFM_SIZE = 0x80000000;
        private const uint CFM_COLOR = 0x40000000;
        private const uint CFM_FACE = 0x20000000;
        private const uint CFM_OFFSET = 0x10000000;
        private const uint CFM_CHARSET = 0x08000000;
        private const uint CFM_SUBSCRIPT = CFE_SUBSCRIPT | CFE_SUPERSCRIPT;
        private const uint CFM_SUPERSCRIPT = CFM_SUBSCRIPT;

        private const byte CFU_UNDERLINENONE = 0x00000000;
        private const byte CFU_UNDERLINE = 0x00000001;
        private const byte CFU_UNDERLINEWORD = 0x00000002; /* (*) displayed as ordinary underline	*/
        private const byte CFU_UNDERLINEDOUBLE = 0x00000003; /* (*) displayed as ordinary underline	*/
        private const byte CFU_UNDERLINEDOTTED = 0x00000004;
        private const byte CFU_UNDERLINEDASH = 0x00000005;
        private const byte CFU_UNDERLINEDASHDOT = 0x00000006;
        private const byte CFU_UNDERLINEDASHDOTDOT = 0x00000007;
        private const byte CFU_UNDERLINEWAVE = 0x00000008;
        private const byte CFU_UNDERLINETHICK = 0x00000009;
        private const byte CFU_UNDERLINEHAIRLINE = 0x0000000A; /* (*) displayed as ordinary underline	*/

        #endregion

        #endregion

        public delegate void InlineLinkClickedEventHandler(object sender, InlineLinkClickedEventArgs e);
        public event InlineLinkClickedEventHandler InlineLinkClicked;

        public RicherTextBox()
        {
            DetectUrls = false;
            LinkClicked += HandleRichTextBoxLinks;
        }

        private void HandleRichTextBoxLinks(object sender, LinkClickedEventArgs e)
        {
            if (InlineLinkClicked != null)
            {
                int Split = e.LinkText.LastIndexOf('@');

                string Label = e.LinkText.Substring(0, Split);
                string Link = e.LinkText.Substring(Split + 1);
                InlineLinkClicked(this, new InlineLinkClickedEventArgs(Label, Link));
            }
        }

        [DefaultValue(false)]
        public new bool DetectUrls
        {
            get { return base.DetectUrls; }
            set { base.DetectUrls = value; }
        }

        public void Add(string text)
        {
            SelectedText = text;
            Select(SelectionStart + text.Length, 0);
        }

        public void AddLine(string text)
        {
            Add(text + "\n");
        }

        public void InsertLinkLine(string text, string hyperlink)
        {
            InsertLink(text, hyperlink);
            Add("\n");
        }

        public void InsertLink(string hyperlink)
        {
            // Label using the link address
            InsertLink(hyperlink, hyperlink, SelectionStart);
        }

        public void InsertLink(string text, string hyperlink)
        {
            InsertLink(text, hyperlink, SelectionStart);
        }

        private void InsertLink(string text, string hyperlink, int position)
        {
            if (position < 0 || position > Text.Length)
                throw new ArgumentOutOfRangeException("Invalid text position");

            SelectionStart = position;
            SelectedRtf = @"{\rtf1\ansi " + text + @"\v @" + hyperlink + @"\v0}";
            Select(position, text.Length + hyperlink.Length + 1);
            SetSelectionLink(true);
            Select(position + text.Length + hyperlink.Length + 1, 0);
        }

        private void SetSelectionLink(bool link)
        {
            SetSelectionStyle(CFM_LINK, link ? CFE_LINK : 0);
        }

        private void SetSelectionStyle(uint mask, uint effect)
        {
            CHARFORMAT2_STRUCT cf = new CHARFORMAT2_STRUCT();
            cf.cbSize = (uint)Marshal.SizeOf(cf);
            cf.dwMask = mask;
            cf.dwEffects = effect;

            IntPtr wpar = new IntPtr(SCF_SELECTION);
            IntPtr lpar = Marshal.AllocCoTaskMem(Marshal.SizeOf(cf));
            Marshal.StructureToPtr(cf, lpar, false);

            SendMessage(Handle, EM_SETCHARFORMAT, wpar, lpar);

            Marshal.FreeCoTaskMem(lpar);
        }

        private int GetSelectionStyle(uint mask, uint effect)
        {
            CHARFORMAT2_STRUCT cf = new CHARFORMAT2_STRUCT();
            cf.cbSize = (uint)Marshal.SizeOf(cf);
            cf.szFaceName = new char[32];

            IntPtr wpar = new IntPtr(SCF_SELECTION);
            IntPtr lpar = Marshal.AllocCoTaskMem(Marshal.SizeOf(cf));
            Marshal.StructureToPtr(cf, lpar, false);

            IntPtr res = SendMessage(Handle, EM_GETCHARFORMAT, wpar, lpar);

            cf = (CHARFORMAT2_STRUCT)Marshal.PtrToStructure(lpar, typeof(CHARFORMAT2_STRUCT));

            int state = -1;

            // dwMask holds the information which properties are consistent throughout the selection:
            if ((cf.dwMask & mask) == mask)
            {
                if ((cf.dwEffects & effect) == effect)
                    state = 1;
                else
                    state = 0;
            }

            Marshal.FreeCoTaskMem(lpar);
            return state;
        }

        public void BeginUpdate()
        {
            SendMessage(Handle, WM_SETREDRAW, IntPtr.Zero, IntPtr.Zero);
            OldEventMask = SendMessage(Handle, EM_SETEVENTMASK, IntPtr.Zero, IntPtr.Zero);
        }

        public void EndUpdate()
        {
            SendMessage(Handle, WM_SETREDRAW, (IntPtr)1, IntPtr.Zero);
            SendMessage(Handle, EM_SETEVENTMASK, IntPtr.Zero, OldEventMask);
        }
    }

    public class InlineLinkClickedEventArgs : EventArgs
    {
        public InlineLinkClickedEventArgs(string label, string link)
        {
            Label = label;
            Link = link;
        }

        public string Label { get; }
        public string Link { get; }
    }
}
