<h3 align="center">C Simulator for an Hardware Accelerator of Convolutional Neural Networks (CNNs)</h3>

<div align="center">

[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](/LICENSE)

</div>

---

<p align="left"> The convolution layer architecture was defined by Wang et. al in "Hardware Architectures for Deep Convolutional Neural Network".
    <br> 
</p>

## Implements:
- Parallel FIR filtering 
- Max Pooling Layer
- Command Line Stride Visualization 

## Usage:
1. Compile the simulator:
```bash
gcc -g *.c -o sim
```

2. Generate input shapes:
```bash
# Generate all shapes (square, circle, triangle, pentagon, star)
python generate_shapes.py [image_size]

# Generate specific shape
python generate_shapes.py [image_size] --shape [shape_name]
# Available shapes: square, circle, triangle, pentagon, star
```

3. Run the simulator:
```bash
# Basic usage with shape selection
./sim [image_size] [shape] 

# With debug visualization and speed control
./sim [image_size] [shape] --debug [speed_option]

# Examples:
./sim 100 square              # Run with square input
./sim 100 circle              # Run with circle input  
./sim 100 triangle            # Run with triangle input
./sim 100 pentagon            # Run with pentagon input
./sim 100 star                # Run with star input

# Debug modes with different speeds:
./sim 100 circle --debug -f   # Fast debug mode
./sim 100 triangle --debug -m # Medium debug mode
./sim 100 star --debug -s     # Slow debug mode
./sim 100 pentagon --debug --step # Manual step-through mode
``` 
## Vertical Edge Detection Example
https://drive.google.com/file/d/1Yx-8amAuLGYSD3KCUU9ZN4mJe844WbZr/view?usp=sharing 


<img src="Circle.gif" alt="Example" width="500"/>