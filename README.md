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
gcc -o *sim sim*.c fcu.c -lm
```

2. Generate input shapes:
```bash
python generate_shapes.py [image_width] [square_length] [circle_radius]
```

3. Run the simulator:
```bash
# Fast mode
./sim 100 -f

# Medium mode  
./sim 100 -m

# Slow mode (default)
./sim 100 -s

# Manual step-through mode
./sim 100 --step
``` 
