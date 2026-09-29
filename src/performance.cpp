#include "performance.h"
#include <opencv2/opencv.hpp>

BenchmarkResult run_sequential(
    std::vector<std::string>& image_files,
    std::string output_folder)
{
    std::cout << "\n[Sequential] Starting sequential processing...\n";
    auto start = std::chrono::high_resolution_clock::now();
    int processed = 0;

    for (int i = 0; i < static_cast<int>(image_files.size()); i++) {
        cv::Mat img = cv::imread(image_files[i]);
        if (img.empty()) {
            std::cout << "[Sequential] Skipping invalid image: "
                      << image_files[i] << "\n";
            continue;
        }

        img = apply_grayscale(img);
        img = apply_brightness(img, 1.2, 10);

        std::string filename = image_files[i].substr(
            image_files[i].find_last_of("/\\") + 1
        );
        std::string out_path = output_folder + "/seq_" + filename;

        if (!cv::imwrite(out_path, img)) {
            std::cout << "[Sequential] ERROR saving image: " << filename << "\n";
            continue;
        }

        std::cout << "[Sequential] Processed image " << i
                  << " -> " << filename << "\n";
        processed++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    std::cout << "[Sequential] Done. Time: " << elapsed << "s\n";

    BenchmarkResult result{};
    result.sequential_time = elapsed;
    result.image_count = processed;
    return result;
}

void print_comparison(BenchmarkResult& result) {
    if (result.parallel_time <= 0.0) {
        std::cout << "\nPerformance comparison unavailable: parallel time was not recorded.\n";
        result.speedup = 0.0;
        return;
    }

    result.speedup = result.sequential_time / result.parallel_time;

    std::cout << "\n";
    std::cout << "==========================================\n";
    std::cout << "     PERFORMANCE COMPARISON RESULTS       \n";
    std::cout << "==========================================\n";
    std::cout << "Images Processed : " << result.image_count << "\n";
    std::cout << "Sequential Time  : " << std::fixed << std::setprecision(4)
              << result.sequential_time << "s\n";
    std::cout << "Parallel Time    : " << std::fixed << std::setprecision(4)
              << result.parallel_time << "s\n";
    std::cout << "Speedup          : " << std::fixed << std::setprecision(2)
              << result.speedup << "x\n";
    std::cout << "==========================================\n\n";
}
