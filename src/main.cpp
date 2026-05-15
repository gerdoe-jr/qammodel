#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <qammodel/awgn_channel.h>
#include <qammodel/bit_sequence.h>
#include <qammodel/demodulator.h>
#include <qammodel/modulator.h>
#include <qammodel/random_bit_sequence.h>

constexpr std::size_t max_constellation_points = 10'000;

template<typename Symbol>
void write_constellation_csv(const std::string &path, const std::vector<Symbol> &symbols,
	std::size_t max_points = max_constellation_points) {
	std::ofstream csv{path};

	if(!csv) {
		throw std::runtime_error("Cannot open constellation CSV file: " + path);
	}

	csv << "real,imag\n";

	const std::size_t count = std::min(symbols.size(), max_points);

	for(std::size_t i = 0; i < count; ++i) {
		csv << std::setprecision(12) << symbols[i].real() << ',' << std::setprecision(12) << symbols[i].imag()
		    << '\n';
	}
}

template<std::uint8_t modulation_level>
void perform_simulation(random_bit_sequence_generator::seed_type seed, std::size_t frame_length,
	const std::vector<double> &noise_variances, const std::string &output_prefix) {
	if(noise_variances.empty()) {
		throw std::invalid_argument("noise_variances must not be empty");
	}

	random_bit_sequence_generator rbsg{seed};
	qam_modulator<modulation_level> modulator;
	qam_demodulator<modulation_level> demodulator;
	awgn_channel channel{seed};

	const bit_sequence clean_bits = rbsg.generate(frame_length);
	const std::vector<symbol_t> clean_symbols = modulator.modulate(clean_bits);

	std::ofstream ber_csv{output_prefix + "_ber.csv"};

	if(!ber_csv) {
		throw std::runtime_error("Cannot open BER CSV file: " + output_prefix + "_ber.csv");
	}

	ber_csv << "variance,ber\n";

	write_constellation_csv(output_prefix + "_constellation_clean.csv", clean_symbols);

	std::vector<symbol_t> noisy_first_symbols;
	std::vector<symbol_t> noisy_last_symbols;

	const double first_variance = noise_variances.front();
	const double last_variance = noise_variances.back();

	for(const double variance : noise_variances) {
		std::vector<symbol_t> noisy_symbols = clean_symbols;

		channel.add_noise(noisy_symbols, variance);

		if(variance == first_variance) {
			noisy_first_symbols = noisy_symbols;
		}

		if(variance == last_variance) {
			noisy_last_symbols = noisy_symbols;
		}

		const bit_sequence restored_bits = demodulator.demodulate(noisy_symbols);
		const double ber = bit_error_rate(clean_bits, restored_bits);

		ber_csv << std::setprecision(12) << variance << ',' << std::setprecision(12) << ber << '\n';
	}

	write_constellation_csv(output_prefix + "_constellation_noisy_first.csv", noisy_first_symbols);

	write_constellation_csv(output_prefix + "_constellation_noisy_last.csv", noisy_last_symbols);
}

int main() {
	try {
		const std::string output_dir = "data/";

		std::filesystem::create_directories(output_dir);

		const std::size_t frame_length = 6 * 100000;
		const random_bit_sequence_generator::seed_type seed = 67;

		const std::vector<double> noise_variances{
			0.0001, 0.0002, 0.0005, 0.001, 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1.0};

		perform_simulation<1>(seed, frame_length, noise_variances, output_dir + "qam4");
		perform_simulation<2>(seed, frame_length, noise_variances, output_dir + "qam16");
		perform_simulation<3>(seed, frame_length, noise_variances, output_dir + "qam64");

		std::cout << "Simulation data saved" << std::endl;

	} catch(const std::exception &exception) {
		std::cerr << "Error: " << exception.what() << std::endl;
		return 1;
	}

	return 0;
}