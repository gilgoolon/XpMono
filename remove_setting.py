from glob import glob

for path in glob("C:\\Users\\alper\\OneDrive\\Documents\\XpMono\\**\\*.vcxproj", recursive=True):
    print(path)
    lines = []
    with open(path, 'r') as f:
        for line in f.readlines():
            if "<ImageHasSafeExceptionHandlers>false</ImageHasSafeExceptionHandlers>" not in line:
                lines.append(line)
            else:
                print("found")
    with open(path, 'w') as f:
        f.writelines(lines)
