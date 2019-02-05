keywords = {'idle': 0x1, 'speed': 0x2, 'func': 0x3, 'acc': 0x4, 'delay': 0x5, 'block': 0x6}
counter = 0

with open('Data.h', 'w') as datafile:
    datafile.write('uint16_t commands[] = {')
    with open('Data.txt', 'r') as inputfile:
        for line in inputfile:
            lineParts = line.split()
            if (len(lineParts) > 0):
                if (counter > 0):
                    datafile.write(', ')
                datafile.write(hex(keywords[lineParts[0]]))
                counter = counter + 1
                for n, item in enumerate(lineParts[1:]):
                    datafile.write(", ")
                    datafile.write(hex(int(item)))
                    counter = counter + 1
                    #if (n < len(lineParts[1:]) - 1):
                    #    datafile.write(", ")
        datafile.write('};\n')
        datafile.write('#define COMMAND_LENGTH ' + str(counter))
