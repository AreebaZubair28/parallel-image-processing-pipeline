# Parallel Image Processing Pipeline

A multithreaded image-processing pipeline developed as an Operating Systems course project at FAST-NUCES Karachi. The project demonstrates producer-consumer synchronization and pipeline concurrency using POSIX threads, mutexes, semaphores, bounded buffers, and OpenCV.

## Pipeline Architecture

Images move through four concurrent stages:

`Loader -> Filter -> Enhancer -> Saver`

Each stage communicates through a bounded shared buffer protected by a mutex and POSIX semaphores. With the current configuration, one worker runs at each stage, allowing different images to be processed concurrently across the pipeline.

## Features

- Four-stage concurrent image-processing pipeline
- POSIX threads (`pthread`)
- Producer-consumer synchronization with mutexes and semaphores
- Bounded shared queues between pipeline stages
- Atomic progress counters
- OpenCV-based grayscale filtering and brightness/contrast enhancement
- Sequential and pipeline execution modes for timing comparison
- Optional Qt interface for live pipeline progress
- CMake-based build configuration

## Tech Stack

- C++17
- POSIX Threads
- POSIX Semaphores
- OpenCV
- Qt5 Widgets (optional)
- CMake
- Linux

## Project Structure

```text
parallel-image-processing-pipeline/
├── src/
│   ├── main.cpp
│   ├── pipeline.cpp
│   ├── pipeline.h
│   ├── buffer.h
│   ├── filters.cpp
│   ├── filters.h
│   ├── performance.cpp
│   ├── performance.h
│   ├── gui.cpp
│   └── gui.h
├── input_images/
├── output_images/
├── docs/
│   └── project-report.pdf
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Build

### Requirements

- Linux
- C++ compiler with C++17 support
- CMake 3.10+
- OpenCV development libraries
- POSIX threads and semaphores
- Qt5 Widgets (optional; the project also builds without the GUI)

### Compile

```bash
mkdir build
cd build
cmake ..
make
```

## Run

Add JPG, JPEG, PNG, or BMP images to `input_images/` from the project root.

From the project root, run:

```bash
./build/pipeline
```

To compare sequential processing with the concurrent pipeline:

```bash
./build/pipeline --benchmark
```

Processed images are written to `output_images/`.

## Image Processing

The pipeline currently applies:

1. **Filter stage:** grayscale conversion
2. **Enhancer stage:** brightness and contrast adjustment

Additional OpenCV functions for blur, edge detection, and sharpening are included in `filters.cpp` and can be used to extend the processing stages.

## Performance Comparison

Benchmark mode first processes the same image set sequentially and then runs the concurrent pipeline. It reports the image count, sequential execution time, pipeline execution time, and measured speedup.

Performance results depend on image size, image count, storage speed, CPU resources, and the cost of each processing stage, so no fixed speedup is assumed.

## Results

### Pipeline Execution

The pipeline processes multiple images through the Loader, Filter, Enhancer, and Saver stages concurrently.

![Parallel image-processing pipeline execution](screenshots/pipeline-execution.jpg)

### Performance Benchmark

The project includes a benchmark mode that compares sequential image processing with the concurrent pipeline.

![Sequential vs parallel performance results](screenshots/benchmark-results.jpg)

The recorded test processed three images in approximately **0.7721 seconds sequentially** and **0.7164 seconds using the pipeline**, producing a measured speedup of approximately **1.08x**. Performance varies depending on the workload and system configuration.

### Sequential and Parallel Execution

Benchmark mode first processes the images sequentially and then executes the parallel pipeline using the same image set.

![Sequential and parallel benchmark execution](screenshots/benchmark-execution.jpg)

## Project Contribution

This was a team Operating Systems coursework project developed at FAST-NUCES Karachi by Areeba Zubair, Aqsa Zubair, and Maryam.

**Areeba Zubair's contribution** focused on thread pool implementation, buffer management, and batch processing for the parallel image-processing pipeline.

The repository presents the complete team project while distinguishing individual contribution rather than claiming sole authorship.

## Academic Context

The original project was developed for the Operating Systems course at FAST-NUCES Karachi. This repository is a cleaned portfolio version of the original coursework project.

## Notes

- The current configuration uses one worker thread per pipeline stage.
- Pipeline concurrency comes from different stages processing different images at the same time.
- Qt is optional. If Qt5 Widgets is not found, CMake builds the command-line version.
