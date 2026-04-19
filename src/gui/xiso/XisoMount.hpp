#pragma once

#include <memory>
#include <string>

class XisoMount
{
public:
	XisoMount();
	~XisoMount();

	XisoMount(const XisoMount&) = delete;
	XisoMount& operator=(const XisoMount&) = delete;

	static bool IsDokanAvailable(std::string& error);

	bool Mount(const std::string& isoPath, const std::wstring& mountPoint, std::string& error);
	void Unmount();

	bool IsMounted() const;
	const std::string& GetMountedDefaultXbePath() const;
	const std::string& GetSourcePath() const;
	const std::wstring& GetMountPoint() const;

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};
