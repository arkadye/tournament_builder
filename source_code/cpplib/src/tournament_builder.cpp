#include "tournament_builder.hpp"
#include "tournament_builder_json_obj.hpp"

#include "nlohmann/json.hpp"

#include <sstream>
#include <fstream>

namespace tournament_builder
{
	namespace internal_tournament_builder_lightweight
	{
		template <typename T>
		void wrap_ostream(T&& arg, std::ostream& output)
		{
			nlohmann::json result = make_tournament_json(std::forward<T>(arg));
			output << result;
		}

		template <typename T>
		std::string wrap_str(T&& arg)
		{
			std::ostringstream out_stream;
			wrap_ostream(std::forward<T>(arg), out_stream);
			return out_stream.str();
		}

		template <typename T>
		void wrap_path(T&& arg, const std::filesystem::path& output)
		{
			std::ofstream out_file{ output };
			wrap_ostream(std::forward<T>(arg), out_file);
		}
	}

	std::string make_tournament_str(std::string_view input) { return internal_tournament_builder_lightweight::wrap_str(input); }
	std::string make_tournament_str(const std::filesystem::path& input) { return internal_tournament_builder_lightweight::wrap_str(input); }
	std::string make_tournament_str(std::istream& input) { return internal_tournament_builder_lightweight::wrap_str(input); }
	std::string make_tournament_str(const nlohmann::json& input) { return internal_tournament_builder_lightweight::wrap_str(input); }
	std::string make_tournament_str(const tournament_builder::World& input) { return internal_tournament_builder_lightweight::wrap_str(input); }
	void make_tournament_path(std::string_view input, const std::filesystem::path& output) { return internal_tournament_builder_lightweight::wrap_path(input, output); }
	void make_tournament_path(const std::filesystem::path& input, const std::filesystem::path& output) { return internal_tournament_builder_lightweight::wrap_path(input, output); }
	void make_tournament_path(std::istream& input, const std::filesystem::path& output) { return internal_tournament_builder_lightweight::wrap_path(input, output); }
	void make_tournament_path(const nlohmann::json& input, const std::filesystem::path& output) { return internal_tournament_builder_lightweight::wrap_path(input, output); }
	void make_tournament_path(const tournament_builder::World& input, const std::filesystem::path& output) { return internal_tournament_builder_lightweight::wrap_path(input, output); }
	void make_tournament_ostream(std::string_view input, std::ostream& output) { return internal_tournament_builder_lightweight::wrap_ostream(input, output); }
	void make_tournament_ostream(const std::filesystem::path& input, std::ostream& output) { return internal_tournament_builder_lightweight::wrap_ostream(input, output); }
	void make_tournament_ostream(std::istream& input, std::ostream& output) { return internal_tournament_builder_lightweight::wrap_ostream(input, output); }
	void make_tournament_ostream(const nlohmann::json& input, std::ostream& output) { return internal_tournament_builder_lightweight::wrap_ostream(input, output); }
	void make_tournament_ostream(const tournament_builder::World& input, std::ostream& output) { return internal_tournament_builder_lightweight::wrap_ostream(input, output); }

	bool test_input_file(const std::filesystem::path& input, const std::filesystem::path& reference)
	{
		nlohmann::json processed = make_tournament_json(input);
		std::ifstream ref_file{ reference };
		if (!ref_file.is_open())
		{
			std::cerr << std::format("Could not open reference file '{}'\n", std::filesystem::absolute(reference).generic_string());
			return false;
		}

		nlohmann::json ref_object;
		ref_file >> ref_object;
		const bool result = processed == ref_object;
		std::cout << std::format("Input file '{}' {} output file '{}'", input.generic_string(), result ? "matches" : "does not match", reference.generic_string());
		if (!result)
		{
			std::cout << "\nExpected:\n" << ref_object << "'\nGot:\n" << processed << '\n';
		}
		return result;
	}
}