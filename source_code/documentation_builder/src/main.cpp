#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <cassert>
#include <string_view>
#include <algorithm>
#include <ranges>
#include <tuple>

#include "cxxopts.hpp"

namespace stf = std::filesystem;

static constexpr std::string_view MD = ".md";
static constexpr std::string_view NAV_MAP = "___NAV-MAP___";
static constexpr std::string_view INDENT = "  ";
static constexpr std::string_view README = "README.MD";

class Folder;
class File
{
	std::string title;
	stf::path filename;
public:
	explicit File(const stf::path& path) : title{ path.filename().string() }, filename{ path.filename() } {}
	static File make(const std::filesystem::path &path);
	std::string_view get_title() const noexcept { return title; }
	const stf::path& get_filename() const noexcept {return filename; }
	bool is_readme() const;
	int transfer_file(const Folder& top_folder, stf::path from, stf::path to, bool for_real, bool force) const;
	bool check_can_transfer(const stf::path& destination) const;
	auto operator<=>(const File& other) const noexcept = default;
};

class Folder
{
	std::string title;
	stf::path path_fragment;
	std::set<Folder> subfolders;
	std::set<File> files;
	void make_navigation_impl(std::vector<std::string>& result, stf::path path_here, const stf::path& target, std::string_view indent) const;
	int transfer_folder_impl(const Folder& top_folder, const stf::path& from, const stf::path& to, bool for_real, bool force) const;
public:
	explicit Folder(const stf::path& path) : title{ path.stem().string() }, path_fragment{ path.stem() } {}
	std::string_view get_title() const noexcept { return title; }
	const stf::path& get_path_fragment() const { return path_fragment; }
	static Folder make(const stf::path& path);
	int transfer_folder(const stf::path& from, const stf::path& to, bool for_real, bool force) const;
	bool check_can_transfer(const stf::path& destination) const;
	std::string make_navigation(const stf::path& target) const;
	auto operator<=>(const Folder& other) const noexcept { return std::tuple{ title,path_fragment } <=> std::tuple{ other.title, other.path_fragment }; };
};

int main(int argc, char** argv)
{
	cxxopts::Options options_config{ "This simple tool moves Markdown files through a folder structure. It can add navigation maps with the macro ___NAV-MAP___." };
	options_config.add_options()
		("i,input", "Input path - the top level path of documentation to read from.", cxxopts::value<stf::path>())
		("o,output", "Output path - the location to write the documentation to", cxxopts::value<stf::path>())
		("r,real", "Real - set this argument to actually write things. Otherwise it will report which files will be updated only.")
		("f,force", "Force - overwrite files even if the destination is newer than the source.")
		("h,help", "Display help");

	auto options = options_config.parse(argc, argv);

	if (options.contains("help"))
	{
		std::cout << options_config.help() << '\n';
		return EXIT_SUCCESS;
	}

	if (!options.contains("input"))
	{
		std::cerr << "No 'input' argument was specified.";
		return EXIT_FAILURE;
	}

	if (!options.contains("output"))
	{
		std::cerr << "No 'input' argument was specified.";
		return EXIT_FAILURE;
	}

	const stf::path source = *options.as_optional<stf::path>("input");
	const stf::path destination = *options.as_optional<stf::path>("output");
	const Folder folder_structure = Folder::make(source);
	if (!folder_structure.check_can_transfer(destination))
	{
		return EXIT_FAILURE;
	}
	return folder_structure.transfer_folder(source, destination, options.contains("real"), options.contains("force"));
}

int Folder::transfer_folder(const stf::path& from, const stf::path& to, bool for_real, bool force) const
{
	return transfer_folder_impl(*this, from, to, for_real, force);
}

int Folder::transfer_folder_impl(const Folder& top_folder, const stf::path& from, const stf::path& to, bool for_real, bool force) const
{
	if (for_real && !stf::exists(to))
	{
		stf::create_directory(to);
	}
	for (const File& file : files)
	{
		if (EXIT_SUCCESS != file.transfer_file(top_folder, from, to, for_real, force))
		{
			return EXIT_FAILURE;
		}
	}

	for (const Folder& subfolder : subfolders)
	{	
		subfolder.transfer_folder_impl(top_folder, from / subfolder.get_path_fragment(), to / subfolder.get_path_fragment(), for_real, force);
	}
	return EXIT_SUCCESS;
}

int File::transfer_file(const Folder& top_folder, stf::path from, stf::path to, bool for_real, bool force) const
{
	from /= filename;
	to /= filename;
	std::optional<std::string> nav_map;
	if (force || !stf::exists(to) || stf::last_write_time(from) > stf::last_write_time(to))
	{
		std::cout << std::format("Transferring file '{}' from '{}' to '{}'\n", title, from.string(), to.string());
		if (!for_real) return EXIT_SUCCESS;
		std::vector<std::string> lines;
		std::ifstream input{ from };
		if (!input.is_open()) return EXIT_FAILURE;
		while (!input.eof())
		{
			std::string line;
			std::getline(input, line);
			line.push_back('\n');
			lines.push_back(std::move(line));
		}
		input.close();

		// Remove \n from last line
		if (!lines.empty())
		{
			lines.back().pop_back();
		}

		// Insert our navigation map.
		for (std::string& line : lines)
		{
			if (std::size_t nav_map_loc = line.find(NAV_MAP); nav_map_loc < line.size())
			{
				if (!nav_map.has_value())
				{
					nav_map = top_folder.make_navigation(from);
				}
				assert(nav_map.has_value());
				line = line.replace(nav_map_loc, NAV_MAP.size(), *nav_map);
			}
		}
		std::ofstream output{ to };
		if (!output.is_open())
		{
			std::cerr << std::format("Error! Could not open output file '{}'", to.string());
			return EXIT_FAILURE;
		}
		std::ranges::copy(lines, std::ostream_iterator<std::string>{output});
	}
	
	return EXIT_SUCCESS;
}

std::string trim(std::string_view in)
{
	while (!in.empty() && std::isspace(in.front()))
	{
		in.remove_prefix(1);
	}
	while (!in.empty() && std::isspace(in.back()))
	{
		in.remove_suffix(1);
	}
	return std::string{ in };
}

File File::make(const stf::path& path)
{
	constexpr std::string_view TITLE_PREFIX{ "# " };
	assert(!stf::is_directory(path));
	assert(path.has_extension() && path.extension() == MD);
	File result{ path };
	std::ifstream file{ path };
	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);
		if (line.starts_with(TITLE_PREFIX))
		{
			result.title = trim({line.begin()+TITLE_PREFIX.size(),line.end()});
			break;
		}
	}
	return result;
}

bool is_readme(const stf::path& target) noexcept
{
	auto tu = [](char c) {return static_cast<char>(std::toupper(c)); };
	const stf::path stem = target.filename();
	const std::string stem_str = stem.string();
	return std::ranges::equal(stem_str | std::views::transform(tu), README);
}

bool File::is_readme() const
{
	return ::is_readme(get_filename());
}

Folder Folder::make(const stf::path& path)
{
	assert(std::filesystem::is_directory(path));
	Folder result{ path };
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator{ path })
	{
		if (entry.is_directory())
		{
			result.subfolders.emplace(make(entry.path()));
		}
		else if(entry.path().extension() == MD)
		{
			auto [it, success] = result.files.emplace(File::make(entry.path()));
			assert(success);
			
			if (it->is_readme())
			{
				result.title = it->get_title();
			}
		}
	}
	return result;
}

bool File::check_can_transfer(const stf::path& path) const
{
	const auto filepath = path / filename;
	const bool result = !stf::exists(filepath) || !stf::is_directory(filepath);
	if (!result)
	{
		std::cout << std::format("Cannot transfer file to '{}': target already exists and is not a file.", stf::canonical(path).string());
	}
	return result;
}

bool Folder::check_can_transfer(const stf::path& path) const
{
	if (std::ranges::none_of(files, &File::is_readme))
	{
		std::cerr << std::format("Cannot transfer: source directory for '{}' does not have a readme.md", stf::canonical(path).string());
		return false;
	}
	if (!stf::exists(path)) return true;
	if (!stf::is_directory(path))
	{
		std::cerr << std::format("Cannot transfer: target directory '{}' exists but is not a directory.", stf::canonical(path).string());
		return false;
	}
	if (!std::ranges::all_of(files, [&path](const File& f) {return f.check_can_transfer(path); })) return false;
	return std::ranges::all_of(subfolders, [&path](const Folder& f) {return f.check_can_transfer(path / f.get_path_fragment()); }) ;
}

std::string Folder::make_navigation(const stf::path& target) const
{
	std::vector<std::string> lines;
	make_navigation_impl(lines, "", target, "");
	assert(!lines.empty());
	lines.back().pop_back(); // Remove trailing newline
	std::ostringstream output;
	std::ranges::copy(lines, std::ostream_iterator<std::string>{output});
	return output.str();
}

void Folder::make_navigation_impl(std::vector<std::string>& result, stf::path path_here, const stf::path& target, std::string_view indent) const
{
	path_here /= get_path_fragment();
	
	auto make_entry = [&target, &path_here](const File& file)
		{
			const stf::path filepath = path_here / file.get_filename();
			if (target == filepath)
			{
				return std::format("- **{}** (You are here)\n", file.get_title());
			}
			else
			{
				const stf::path to_target = stf::relative(filepath, target.parent_path());
				return std::format("- [{}]({})\n", file.get_title(), to_target.string());
			}
		};

	// Put the readme.md at the top.
	const auto readme_it = std::ranges::find_if(files, &File::is_readme);
	assert(readme_it != end(files));
	result.emplace_back(std::format("{}{}", indent, make_entry(*readme_it)));

	{
		const std::string target_string = target.string();
		const std::string here_string = path_here.string();
		const bool here_is_on_route_to_target = target_string.starts_with(here_string);

		const bool target_is_readme = is_readme(target);
		const std::string target_folder_string = target.parent_path().string();
		const bool target_is_parent_of_here = target_is_readme && here_string.starts_with(target_folder_string);
		if (!here_is_on_route_to_target && !target_is_parent_of_here)
		{
			return;
		}
	}
	

	auto file_it = begin(files);
	auto folder_it = begin(subfolders);

	auto get_file_next = [&file_it, &folder_it, &fileset = files, &folderset = subfolders]()
		{
			if (folder_it == end(folderset)) return true;
			if (file_it == end(fileset)) return false;
			return file_it->get_title() < folder_it->get_title();
		};

	const std::string next_indent = std::format("{}{}", INDENT, indent);

	while (file_it != end(files) || folder_it != end(subfolders))
	{
		if (file_it == readme_it)
		{
			// We already procesed the readme, so skip it.
			++file_it;
			continue;
		}
		const bool process_file = get_file_next();
		if (process_file)
		{
			const File& file = *file_it;
			result.emplace_back(std::format("{}{}", next_indent, make_entry(file)));
			++file_it;
		}
		else
		{
			const Folder& folder = *folder_it;
			folder.make_navigation_impl(result, path_here, target, next_indent);
			++folder_it;
		}
	}
}