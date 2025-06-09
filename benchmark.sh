#!/bin/zsh

# Complete N-Body Benchmark Script
echo "==================================="
echo "N-Body Benchmark (15s each test)"
echo "==================================="

# Configuration
BODY_COUNTS=(1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100 2200 2300 2400 2500 2600 2700 2800 2900 3000)
TEST_DURATION=15  # seconds
USE_HEADLESS=false  # Set to true if you have headless benchmark compiled

# Parse command line options
while [[ $# -gt 0 ]]; do
    case $1 in
        --headless)
            USE_HEADLESS=true
            shift
            ;;
        --duration)
            TEST_DURATION="$2"
            shift 2
            ;;
        --bodies)
            IFS=',' read -ra BODY_COUNTS <<< "$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--headless] [--duration SECONDS] [--bodies COUNT1,COUNT2,...]"
            exit 1
            ;;
    esac
done

# Remove old results file
rm -f benchmark_results.csv

echo "Test duration: $TEST_DURATION seconds per test"
echo "Body counts: ${BODY_COUNTS[*]}"
echo "Mode: $([ "$USE_HEADLESS" = true ] && echo "Headless" || echo "GUI with auto-close")"
echo ""

# Check which binaries exist
BINARIES=()
if [ -f "./bin/nbody_simulation_serial" ]; then
    BINARIES+=("./bin/nbody_simulation_serial")
    echo "Found: Serial implementation"
fi

if [ -f "./bin/nbody_simulation_omp" ]; then
    BINARIES+=("./bin/nbody_simulation_omp")
    echo "Found: OpenMP implementation"
fi

# Check for headless versions
if [ "$USE_HEADLESS" = true ]; then
    HEADLESS_BINARIES=()
    if [ -f "./bin/headless_benchmark_serial" ]; then
        HEADLESS_BINARIES+=("./bin/headless_benchmark_serial")
        echo "Found: Headless Serial implementation"
    fi
    
    if [ -f "./bin/headless_benchmark_omp" ]; then
        HEADLESS_BINARIES+=("./bin/headless_benchmark_omp")
        echo "Found: Headless OpenMP implementation"
    fi
    
    if [ ${#HEADLESS_BINARIES[@]} -gt 0 ]; then
        BINARIES=("${HEADLESS_BINARIES[@]}")
    fi
fi

if [ ${#BINARIES[@]} -eq 0 ]; then
    echo "Error: No simulation binaries found!"
    if [ "$USE_HEADLESS" = true ]; then
        echo "Expected: ./bin/headless_benchmark_serial or ./bin/headless_benchmark_omp"
    else
        echo "Expected: ./bin/nbody_simulation_serial or ./bin/nbody_simulation_omp"
    fi
    echo "Make sure you have compiled the project."
    exit 1
fi

echo ""

# Function to run GUI benchmark (with auto-close)
run_gui_benchmark() {
    local binary=$1
    local bodies=$2
    local duration=$3
    
    echo "Running: $(basename "$binary") with $bodies bodies for ${duration}s..."
    
    # Start the simulation in background
    $binary $bodies &
    local sim_pid=$!
    
    # Wait for the specified duration
    sleep $duration
    
    # Send graceful shutdown signal
    if kill -0 $sim_pid 2>/dev/null; then
        kill -TERM $sim_pid 2>/dev/null
        sleep 2
        
        # Force kill if still running
        if kill -0 $sim_pid 2>/dev/null; then
            kill -KILL $sim_pid 2>/dev/null
        fi
        
        wait $sim_pid 2>/dev/null
    fi
    
    echo "  ✓ Completed"
}

# Function to run headless benchmark
run_headless_benchmark() {
    local binary=$1
    local bodies=$2
    local duration=$3
    
    echo "Running: $(basename "$binary") with $bodies bodies for ${duration}s..."
    
    $binary $bodies $duration
    
    echo "  ✓ Completed"
}

# Main benchmark loop
total_tests=$((${#BINARIES[@]} * ${#BODY_COUNTS[@]}))
current_test=0
start_time=$(date +%s)

for binary in "${BINARIES[@]}"; do
    echo "Testing binary: $(basename "$binary")"
    echo "--------------------------------"
    
    for bodies in "${BODY_COUNTS[@]}"; do
        current_test=$((current_test + 1))
        echo "[$current_test/$total_tests]"
        
        if [ "$USE_HEADLESS" = true ]; then
            run_headless_benchmark "$binary" "$bodies" "$TEST_DURATION"
        else
            run_gui_benchmark "$binary" "$bodies" "$TEST_DURATION"
        fi
        
        echo ""
    done
    
    echo "All tests completed for $(basename "$binary")"
    echo ""
done

end_time=$(date +%s)
total_runtime=$((end_time - start_time))

echo "==================================="
echo "Benchmark Complete!"
echo "==================================="

if [ -f "benchmark_results.csv" ]; then
    echo "Results saved to benchmark_results.csv:"
    echo ""
    # Display results in a nice table format
    printf "%-13s | %-6s | %s\n" "Implementation" "Bodies" "Avg FPS"
    printf "%-13s | %-6s | %s\n" "-------------" "------" "-------"
    tail -n +2 benchmark_results.csv | while IFS=',' read -r impl bodies fps; do
        printf "%-13s | %-6s | %.2f\n" "$impl" "$bodies" "$fps"
    done
    echo ""
    echo "Total runtime: ${total_runtime} seconds"
    echo "Expected runtime: $((total_tests * TEST_DURATION)) seconds"
else
    echo "No results file found. Check if the simulations ran properly."
fi
