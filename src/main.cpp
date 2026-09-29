#include <algorithm>
#include <chrono>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include "pipeline.h"
#include "performance.h"

#ifdef GUI_ENABLED
#include "gui.h"
#include <QApplication>
#endif

namespace fs = std::filesystem;

struct PipelineArgs {
    std::vector<std::string>* files;
    std::string output_folder;
    BenchmarkResult* result;
};

void* run_pipeline_thread(void* arg) {
    auto* args = static_cast<PipelineArgs*>(arg);

    auto start = std::chrono::high_resolution_clock::now();
    run_pipeline(*args->files, args->output_folder);
    auto end = std::chrono::high_resolution_clock::now();

    if (args->result) {
        args->result->parallel_time =
            std::chrono::duration<double>(end - start).count();
    }

    return nullptr;
}

bool is_supported_image(const fs::path& path) {
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp";
}

int main(int argc, char* argv[]) {
    std::string input_folder = "input_images";
    std::string output_folder = "output_images";
    bool benchmark_mode = false;

    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--benchmark") {
            benchmark_mode = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << "   Parallel Image Processing Pipeline   \n";
    std::cout << "   Operating Systems Course Project     \n";
    std::cout << "========================================\n\n";
    std::cout << "Input folder  : " << input_folder << "\n";
    std::cout << "Output folder : " << output_folder << "\n\n";

    if (!fs::exists(input_folder) || !fs::is_directory(input_folder)) {
        std::cerr << "ERROR: Input folder does not exist: " << input_folder << "\n";
        return 1;
    }

    std::error_code ec;
    fs::create_directories(output_folder, ec);
    if (ec) {
        std::cerr << "ERROR: Could not create output folder: "
                  << output_folder << "\n";
        return 1;
    }

    std::vector<std::string> image_files;
    for (const auto& entry : fs::directory_iterator(input_folder)) {
        if (entry.is_regular_file() && is_supported_image(entry.path())) {
            image_files.push_back(entry.path().string());
        }
    }

    std::sort(image_files.begin(), image_files.end());

    if (image_files.empty()) {
        std::cout << "ERROR: No supported images found in " << input_folder << "\n";
        std::cout << "Add JPG, JPEG, PNG, or BMP files and run the program again.\n";
        return 1;
    }

    std::cout << "Found " << image_files.size() << " image(s) to process.\n\n";

    if (benchmark_mode) {
        std::cout << "Running in BENCHMARK mode...\n";
        std::cout << "Step 1: Sequential processing\n";
        BenchmarkResult result = run_sequential(image_files, output_folder);

        std::cout << "\nStep 2: Parallel pipeline processing\n";

#ifdef GUI_ENABLED
        QApplication app(argc, argv);
        PipelineWindow window(static_cast<int>(image_files.size()));
        window.show();

        PipelineArgs args = {&image_files, output_folder, &result};
        pthread_t pipeline_thread;
        if (pthread_create(&pipeline_thread, nullptr, run_pipeline_thread, &args) != 0) {
            std::cerr << "ERROR: Could not create pipeline thread.\n";
            return 1;
        }

        app.exec();
        pthread_join(pipeline_thread, nullptr);
#else
        auto start = std::chrono::high_resolution_clock::now();
        run_pipeline(image_files, output_folder);
        auto end = std::chrono::high_resolution_clock::now();
        result.parallel_time =
            std::chrono::duration<double>(end - start).count();
#endif

        print_comparison(result);
        return 0;
    }

#ifdef GUI_ENABLED
    QApplication app(argc, argv);
    PipelineWindow window(static_cast<int>(image_files.size()));
    window.show();

    BenchmarkResult dummy{};
    PipelineArgs args = {&image_files, output_folder, &dummy};
    pthread_t pipeline_thread;
    if (pthread_create(&pipeline_thread, nullptr, run_pipeline_thread, &args) != 0) {
        std::cerr << "ERROR: Could not create pipeline thread.\n";
        return 1;
    }

    int result = app.exec();
    pthread_join(pipeline_thread, nullptr);
    return result;
#else
    run_pipeline(image_files, output_folder);
    return 0;
#endif
}
