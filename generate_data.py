params = dict()
with open("conf.txt") as f:
    for line in f.readlines():
        splited_line = line.split("=")
        if len(splited_line) == 2:
            params[splited_line[0].strip()] = splited_line[1].strip()

h, w = int(params["height"]), int(params["width"])
print(h, w)
with open("heat_map.txt", "w") as f:
    for i in range(h):
        for j in range(w):
            if i == 0 or i == w - 1 or j == 0 or j == h - 1:
                f.write("100" + (" " if j != w - 1 else ""))
            else:
                f.write("0 ")
        f.write("\n")
