import os
import imageio


def create_png_from_txt(file_name, program_name, out_put_file):
    handler = open(file_name)
    line_list = handler.readlines()
    x, y = line_list[len(line_list) - 2].strip().split()[0], line_list[len(line_list) - 2].strip().split()[1]
    os.system("gnuplot -e \"filename='{}'\" -e \"outfile='{}'\" -e \"x={}\" -e \"y={}\" {}".format(file_name, out_put_file, x, y,program_name))

def dir_loop():
    for file in os.listdir("./"):
        if file.endswith(".txt"):
            create_png_from_txt(file, "a.pl", file.replace(".txt", ".png"))

def create_gif():
    images = []
    for file in os.listdir("./"):
        if file.endswith(".png"):
            images.append(imageio.imread(file))
    imageio.mimsave("mygif.gif", images)

dir_loop()
create_gif()
