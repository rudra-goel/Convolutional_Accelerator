import subprocess
import sys
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


def main():
    if len(sys.argv) != 4:
        print("Usage: python generate_shapes.py <image_dim> <square_size> <radius>")
        print("Example: python generate_shapes.py 1000 100 100")
        sys.exit(1)
    
    try:
        image_dim = int(sys.argv[1])
        square_size = int(sys.argv[2])
        radius = int(sys.argv[3])
        
        if image_dim <= 0 or square_size < 0 or radius < 0:
            print("Error: All parameters must be positive integers")
            sys.exit(1)
            
        if square_size >= image_dim // 2:
            print("Warning: square_size is very large compared to image_dim")
            
        if radius >= image_dim // 2:
            print("Warning: radius is very large compared to image_dim")
            
    except ValueError:
        print("Error: All arguments must be integers")
        sys.exit(1)
    
    gen_square(image_dim, square_size)
    gen_circle(image_dim, radius)


main()