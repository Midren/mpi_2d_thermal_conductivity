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
    images = list(filter(lambda x: x.endswith(".png"), os.listdir("./")))
    images.sort(key=lambda x: int(x.split("_")[1].split(".")[0]))
    images = map(lambda x: imageio.imread(x), images)
    imageio.mimsave("mygif.gif", images, duration=0.025, subrectangles=True)

#dir_loop()
create_gif()
