import subprocess
import sys
import math
#create a txt file
def gen_square(image_dim, square_size):
    try:
        with open("inputs/square.txt", "w") as f:
            for i in range(image_dim):
                for j in range(image_dim):
                    if (i >= square_size and i < image_dim - square_size) and (j >= square_size and j < image_dim - square_size):
                        f.write("255\t")
                    else:
                        f.write("0\t")
                f.write("\n")
        print("Square generated successfully.")
    except Exception as e:
        print(f"Error generating square: {e}")

def gen_circle(image_dim, radius):
    try:
        with open("inputs/circle.txt", "w") as f:
            for i in range(image_dim):
                for j in range(image_dim):
                    if ((i - image_dim // 2) ** 2 + (j - image_dim // 2) ** 2) <= radius ** 2:
                        f.write("255\t")
                    else:
                        f.write("0\t")
                f.write("\n")
        print("Circle generated successfully.")
    except Exception as e:
        print(f"Error generating circle: {e}")

def gen_triangle(image_dim, size):
    try:
        with open("inputs/triangle.txt", "w") as f:
            center_x, center_y = image_dim // 2, image_dim // 2
            
            # Triangle vertices (equilateral triangle pointing up)
            height = int(size * math.sqrt(3) / 2)
            vertices = [
                (center_x, center_y - height // 2),  # Top vertex
                (center_x - size // 2, center_y + height // 2),  # Bottom left
                (center_x + size // 2, center_y + height // 2)   # Bottom right
            ]
            
            for i in range(image_dim):
                for j in range(image_dim):
                    if point_in_triangle(j, i, vertices):
                        f.write("255\t")
                    else:
                        f.write("0\t")
                f.write("\n")
        print("Triangle generated successfully.")
    except Exception as e:
        print(f"Error generating triangle: {e}")

def gen_pentagon(image_dim, radius):
    try:
        with open("inputs/pentagon.txt", "w") as f:
            center_x, center_y = image_dim // 2, image_dim // 2
            
            # Pentagon vertices
            vertices = []
            for i in range(5):
                angle = 2 * math.pi * i / 5 - math.pi / 2  # Start from top
                x = center_x + radius * math.cos(angle)
                y = center_y + radius * math.sin(angle)
                vertices.append((x, y))
            
            for i in range(image_dim):
                for j in range(image_dim):
                    if point_in_polygon(j, i, vertices):
                        f.write("255\t")
                    else:
                        f.write("0\t")
                f.write("\n")
        print("Pentagon generated successfully.")
    except Exception as e:
        print(f"Error generating pentagon: {e}")

def gen_star(image_dim, outer_radius, inner_radius):
    try:
        with open("inputs/star.txt", "w") as f:
            center_x, center_y = image_dim // 2, image_dim // 2
            
            # 5-pointed star vertices (alternating outer and inner points)
            vertices = []
            for i in range(10):
                angle = 2 * math.pi * i / 10 - math.pi / 2  # Start from top
                if i % 2 == 0:  # Outer points
                    radius = outer_radius
                else:  # Inner points
                    radius = inner_radius
                x = center_x + radius * math.cos(angle)
                y = center_y + radius * math.sin(angle)
                vertices.append((x, y))
            
            for i in range(image_dim):
                for j in range(image_dim):
                    if point_in_polygon(j, i, vertices):
                        f.write("255\t")
                    else:
                        f.write("0\t")
                f.write("\n")
        print("Star generated successfully.")
    except Exception as e:
        print(f"Error generating star: {e}")

def point_in_triangle(x, y, vertices):
    """Check if point (x, y) is inside triangle using barycentric coordinates"""
    x1, y1 = vertices[0]
    x2, y2 = vertices[1]
    x3, y3 = vertices[2]
    
    denom = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3)
    if abs(denom) < 1e-10:
        return False
    
    a = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denom
    b = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denom
    c = 1 - a - b
    
    return a >= 0 and b >= 0 and c >= 0

def point_in_polygon(x, y, vertices):
    """Check if point (x, y) is inside polygon using ray casting algorithm"""
    n = len(vertices)
    inside = False
    
    p1x, p1y = vertices[0]
    for i in range(1, n + 1):
        p2x, p2y = vertices[i % n]
        if y > min(p1y, p2y):
            if y <= max(p1y, p2y):
                if x <= max(p1x, p2x):
                    if p1y != p2y:
                        xinters = (y - p1y) * (p2x - p1x) / (p2y - p1y) + p1x
                    if p1x == p2x or x <= xinters:
                        inside = not inside
        p1x, p1y = p2x, p2y
    
    return inside


def main():
    if len(sys.argv) < 3:
        print("Usage: python generate_shapes.py <image_dim> <shape> [shape_params...]")
        print("Shapes and parameters:")
        print("  square <square_size>")
        print("  circle <radius>")
        print("  triangle <size>")
        print("  pentagon <radius>")
        print("  star <outer_radius> <inner_radius>")
        print("  all <square_size> <circle_radius> <triangle_size> <pentagon_radius> <star_outer> <star_inner>")
        print("\nExamples:")
        print("  python generate_shapes.py 100 square 30")
        print("  python generate_shapes.py 100 circle 40")
        print("  python generate_shapes.py 100 triangle 50")
        print("  python generate_shapes.py 100 pentagon 35")
        print("  python generate_shapes.py 100 star 40 20")
        print("  python generate_shapes.py 100 all 30 40 50 35 40 20")
        sys.exit(1)
    
    try:
        image_dim = int(sys.argv[1])
        shape = sys.argv[2].lower()
        
        if image_dim <= 0:
            print("Error: Image dimension must be positive")
            sys.exit(1)
        
        if shape == "square":
            if len(sys.argv) != 4:
                print("Error: Square requires 1 parameter: <square_size>")
                sys.exit(1)
            square_size = int(sys.argv[3])
            if square_size < 0:
                print("Error: Square size must be non-negative")
                sys.exit(1)
            if square_size >= image_dim // 2:
                print("Warning: square_size is very large compared to image_dim")
            gen_square(image_dim, square_size)
            
        elif shape == "circle":
            if len(sys.argv) != 4:
                print("Error: Circle requires 1 parameter: <radius>")
                sys.exit(1)
            radius = int(sys.argv[3])
            if radius < 0:
                print("Error: Radius must be non-negative")
                sys.exit(1)
            if radius >= image_dim // 2:
                print("Warning: radius is very large compared to image_dim")
            gen_circle(image_dim, radius)
            
        elif shape == "triangle":
            if len(sys.argv) != 4:
                print("Error: Triangle requires 1 parameter: <size>")
                sys.exit(1)
            size = int(sys.argv[3])
            if size < 0:
                print("Error: Triangle size must be non-negative")
                sys.exit(1)
            gen_triangle(image_dim, size)
            
        elif shape == "pentagon":
            if len(sys.argv) != 4:
                print("Error: Pentagon requires 1 parameter: <radius>")
                sys.exit(1)
            radius = int(sys.argv[3])
            if radius < 0:
                print("Error: Pentagon radius must be non-negative")
                sys.exit(1)
            gen_pentagon(image_dim, radius)
            
        elif shape == "star":
            if len(sys.argv) != 5:
                print("Error: Star requires 2 parameters: <outer_radius> <inner_radius>")
                sys.exit(1)
            outer_radius = int(sys.argv[3])
            inner_radius = int(sys.argv[4])
            if outer_radius < 0 or inner_radius < 0:
                print("Error: Star radii must be non-negative")
                sys.exit(1)
            if inner_radius >= outer_radius:
                print("Error: Inner radius must be smaller than outer radius")
                sys.exit(1)
            gen_star(image_dim, outer_radius, inner_radius)
            
        elif shape == "all":
            if len(sys.argv) != 9:
                print("Error: 'all' requires 6 parameters: <square_size> <circle_radius> <triangle_size> <pentagon_radius> <star_outer> <star_inner>")
                sys.exit(1)
            square_size = int(sys.argv[3])
            circle_radius = int(sys.argv[4])
            triangle_size = int(sys.argv[5])
            pentagon_radius = int(sys.argv[6])
            star_outer = int(sys.argv[7])
            star_inner = int(sys.argv[8])
            
            # Validate all parameters
            params = [square_size, circle_radius, triangle_size, pentagon_radius, star_outer, star_inner]
            if any(p < 0 for p in params):
                print("Error: All parameters must be non-negative")
                sys.exit(1)
            if star_inner >= star_outer:
                print("Error: Star inner radius must be smaller than outer radius")
                sys.exit(1)
                
            gen_square(image_dim, square_size)
            gen_circle(image_dim, circle_radius)
            gen_triangle(image_dim, triangle_size)
            gen_pentagon(image_dim, pentagon_radius)
            gen_star(image_dim, star_outer, star_inner)
            
        else:
            print(f"Error: Unknown shape '{shape}'. Available shapes: square, circle, triangle, pentagon, star, all")
            sys.exit(1)
            
    except ValueError:
        print("Error: All arguments must be integers")
        sys.exit(1)


main()