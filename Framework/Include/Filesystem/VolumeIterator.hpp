#pragma once
#include <filesystem>
#include <queue>
#include <Windows.h>

class VolumeIterator final
{
public:
	explicit VolumeIterator();
	~VolumeIterator();
	VolumeIterator(const VolumeIterator&) = delete;
	VolumeIterator& operator=(const VolumeIterator&) = delete;
	VolumeIterator(VolumeIterator&&) = delete;
	VolumeIterator& operator=(VolumeIterator&&) = delete;

private:
	void add_guid_paths(const std::wstring& guid);
	void try_retrieve_next();

public:
	[[nodiscard]] bool has_next() const;
	[[nodiscard]] std::filesystem::path next();

	[[nodiscard]] static std::filesystem::path get_system_volume();
	[[nodiscard]] static uint32_t get_volume_serial(const std::filesystem::path& volume_path);

private:
	HANDLE m_find_handle;
	bool m_finished_find;
	std::queue<std::filesystem::path> m_entries;
};
