import math

class InvertedPageTableDecoder:
    def __init__(self, processBits, pageBits):
        self.processBits = processBits
        self.pageBits = pageBits

    def getModified(self, entry):
        return (entry >> (self.processBits + self.pageBits + 2)) & 1
    
    def getReferenced(self, entry):
        return (entry >> (self.processBits + self.pageBits + 1)) & 1
    
    def getPresent(self, entry):
        return (entry >> (self.processBits + self.pageBits)) & 1
    
    def setModified(self, entry):
        return entry | (1 << (self.processBits + self.pageBits + 2))
    
    def setReferenced(self, entry):
        return entry | (1 << (self.processBits + self.pageBits + 1))
    
    def setPresent(self, entry):
        return entry | (1 << (self.processBits + self.pageBits))
    
    def clearModified(self, entry):
        return entry & ~(1 << (self.processBits + self.pageBits + 2))
    
    def clearReferenced(self, entry):
        return entry & ~(1 << (self.processBits + self.pageBits + 1))
    
    def getProcessNum(self, entry):
        return (entry >> self.pageBits) & ((1 << self.processBits) - 1)
    
    def getPageNum(self, entry):
        return entry & ((1 << self.pageBits) - 1)
    
    def replaceProcessPage(self, entry, processNum, pageNum):
        temp = entry & ~(((1 << self.processBits) - 1) << self.pageBits)
        temp &= ~((1 << self.pageBits) - 1)
        return temp | (processNum << self.pageBits) | pageNum

def displayInvertedTable(invertedTable, decoder, agingR):
    print("Inverted Page Table (with aging status):")
    print("Frame#\tProcess#\tPage#\tMod\tRef\tAging")
    for i in range(len(invertedTable)):
        entry = invertedTable[i]
        processNum = decoder.getProcessNum(entry)
        pageNum = decoder.getPageNum(entry)
        m = decoder.getModified(entry)
        r = decoder.getReferenced(entry)
        print(f"{i}\t{processNum}\t{pageNum}\t{m}\t{r}\t{agingR[i]}")

def readFile(filename):
    with open(filename, "r") as file:
        lines = file.readlines()

    # First line is the number of bits for things
    #  Virtual Address, Physical Address, Page Size
    virBits = int(lines[0].split()[0])
    phyBits = int(lines[0].split()[1])
    pageBits = int(lines[0].split()[2])

    # Second line is the number of processes - assumed 1 for this and ignored
    numProcesses = int(lines[1])
    
    # Remove the first two lines
    lines = lines[2:]

    return virBits, phyBits, pageBits, lines

def main(filename):
    virBits, phyBits, pageBits, memAccesses = readFile("//Users/billturbadrakh/Desktop/Systems2/lab7/inputTwo.txt")
    numProcesses = 2  # Assuming known number of processes
    processBits = math.ceil(math.log2(numProcesses))
    numPages = 2 ** (virBits - pageBits)
    numFrames = 2 ** (phyBits - pageBits)
    decoder = InvertedPageTableDecoder(processBits, pageBits)
    invertedTable = [0] * numFrames
    agingR = [0] * numFrames
    frameLookup = {}  # Extra credit hash table
    freeFrames = list(range(numFrames))
    
    displayInvertedTable(invertedTable, decoder, agingR)
    numInstructions = 0
    
    for memAccess in memAccesses:
        print("-----------------------------------------------------------")
        numInstructions += 1
        process, command, virMemLoc = memAccess.split()
        process = int(process)
        virMemLoc = int(virMemLoc)
        pageNum = virMemLoc >> pageBits
        offset = virMemLoc & (2 ** pageBits - 1)
        
        frameNum = frameLookup.get((process, pageNum))  # Optimized lookup
        
        if frameNum is None:
            print(" *** Page Fault ***")
            if freeFrames:
                frameNum = freeFrames.pop(0)
            else:
                oldest_frame = min(range(numFrames), key=lambda i: agingR[i])
                oldEntry = invertedTable[oldest_frame]
                oldProcess = decoder.getProcessNum(oldEntry)
                oldPage = decoder.getPageNum(oldEntry)
                if decoder.getModified(oldEntry):
                    print(f"    Writing data from frame {oldest_frame} back to disk page {oldPage} of process {oldProcess}")
                print(f"    Unloading old data from frame {oldest_frame} to disk page {oldPage} of process {oldProcess}")
                del frameLookup[(oldProcess, oldPage)]
                frameNum = oldest_frame
            
            invertedTable[frameNum] = decoder.replaceProcessPage(0, process, pageNum)
            frameLookup[(process, pageNum)] = frameNum
            agingR[frameNum] = 255
            print(f"    Loading new data from disk page {pageNum} of process {process} to frame {frameNum}")
        
        phyMemLoc = (frameNum << pageBits) | offset
        print("--> Physical Location:", phyMemLoc)
        invertedTable[frameNum] = decoder.setReferenced(invertedTable[frameNum])
        if command == 'w':
            invertedTable[frameNum] = decoder.setModified(invertedTable[frameNum])
        
        if numInstructions % 3 == 0:
            print(" ***Aging Buffer Update***")
            for i in range(numFrames):
                agingR[i] >>= 1
                if decoder.getReferenced(invertedTable[i]):
                    agingR[i] |= 1 << 7
                    invertedTable[i] = decoder.clearReferenced(invertedTable[i])
        
        displayInvertedTable(invertedTable, decoder, agingR)

if __name__ == "__main__":
    main('//Users/billturbadrakh/Desktop/Systems2/lab7/inputTwo.txt')
