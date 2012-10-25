#!/usr/bin/python
# Code to convert Dcf generated by the Polymender into a nrrd file 
import sys
import struct
import numpy
import array
'''
python dcfToNrrd.py temp/cube01/cube01-dc-3-0.8.dcf 3
'''


offset=[[0,0,0],[0,0,1],[0,1,0],[0,1,1],[1,0,0],[1,0,1],[1,1,0],[1,1,1]]

data=[]

# main start
def main():
    pointerLocInDataBytes=[10]# starts from 10 to accomodate 'multisign'
    
    fileName=sys.argv[1]
    depth=int(sys.argv[2])
    inpt=[fileName, depth]
    outNrrd =  fileName.split("-")[0]+'-'+fileName.split("-")[2]+'-'+fileName.split("-")[3]+'.nrrd'
    outTxt  = fileName.split("-")[0]+'-'+fileName.split("-")[2]+'-'+fileName.split("-")[3]+'.txt'
    dataBytes=[]
    #Read the data from the .dcf file into dataBytes
    readBytes(fileName, dataBytes)
    #read the lenght of the grid in each direction 
    XLength = convert("i", pointerLocInDataBytes, dataBytes, 4)
    YLength = convert("i", pointerLocInDataBytes, dataBytes, 4)
    ZLength = convert("i", pointerLocInDataBytes, dataBytes, 4)
    global data
    data=numpy.zeros((XLength+1, YLength+1, ZLength+1))
    for i in range (XLength+1):
        for j in range (YLength+1):
            for k in range (ZLength+1):
                data[i][j][k]= 0
    
    #set the current Level
    currLevel=[depth]
    #set the current Location to 0
    currLoc=[0,0,0]
    #Process the nodes
    ProcessNode(pointerLocInDataBytes, dataBytes, inpt, currLevel , currLoc, )
    
    
    #write to the nrrd file 
    
    f = open(outNrrd, 'w ')
    print >>f, "NRRD0001"
    print >>f, "dimension: 3 \ntype: int"
    print >>f, "sizes:", ZLength+1,YLength+1,XLength+1
    print >>f, "spacings: 1 1 1"
    print >>f, "encoding: text"
    print >>f, "\n" 
    
    scalarData=[]
    binaryData=""
    for i in range (XLength+1):
    	for j in range (YLength+1):
    		for k in range (ZLength+1):
    			print >>f, data[i][j][k],
    f.close()
    print '****** File ', outNrrd , 'created.'

    #################################
    #create the txt file with the data
    #################################
    fgarb = open (outTxt,'w')
    for i in range (XLength+1):
    	for j in range (YLength+1):
    		for k in range (ZLength+1):
    			print >>fgarb, data[i][j][k],
    print '****** File ', outTxt, 'created.'
    	
#Process the nodes
def ProcessNode(pointerLocInDataBytes, dataBytes, inpt, currLevel, currLoc):
    global data
    #Compute Node Type (0,1,2)
    NodeType = convert("i", pointerLocInDataBytes, dataBytes, 4)
    
    if NodeType == 0 :
        #decrease the level
        currLevel[0]=currLevel[0]-1
        
        TempCurrLevel = currLevel
        TempCurrLoc = currLoc
        Length = pow(2,currLevel[0])
        for nodeNum in range(8):
            #reset the currLevel, and Location
            TempCurrLevel = currLevel
            TempCurrLoc = currLoc 
            
            #Compute the child locations
            childLoc = [TempCurrLoc[x] + offset[nodeNum][x]*Length for x in range(3)]
            ProcessNode(pointerLocInDataBytes, dataBytes, inpt, TempCurrLevel, childLoc)
        currLevel[0]=currLevel[0]+1
            
    '''
    an empty node <node> contains a 2-byte short integer of value 0 or 1 denoting if it is inside or outside;
    We are flipping everything so 0 is outside and 1 is inside 
    '''
    if NodeType == 1 :
        isInside = convert("h", pointerLocInDataBytes, dataBytes, 2)
        setIntermidiate ( currLoc, currLevel, isInside)
    	
    	
    
    if NodeType == 2 :
        Length = pow(2,currLevel[0])
        
        for nodeNum in range (8):
            nodeLoc  = [currLoc[x] + offset[nodeNum][x] for x in range (3)]
            scalar = convert("h", pointerLocInDataBytes, dataBytes, 2)
            if scalar == 0:
            	data[nodeLoc[0]][nodeLoc[1]][nodeLoc[2]] = 1
            else:
            	data[nodeLoc[0]][nodeLoc[1]][nodeLoc[2]] = 0
            	
        
        for edgeNum in range(12):
            NumPointsOnEdge = convert("i", pointerLocInDataBytes, dataBytes, 4)
            for pointsInEdge in range(NumPointsOnEdge):
                edgeOffset = convert("f", pointerLocInDataBytes, dataBytes, 4)
                N0 = convert("f", pointerLocInDataBytes, dataBytes, 4)
                N1 = convert("f", pointerLocInDataBytes, dataBytes, 4)
                N2 = convert("f", pointerLocInDataBytes, dataBytes, 4)
            

#set the scalar values in the intermediate case by travelling down the Levels
def setIntermidiate ( currLoc, currLevel, scalarVal):
	tempCurrLevel = currLevel[0]
	
	while (tempCurrLevel > 0):
		tempCurrLevel= tempCurrLevel - 1
		for i in range (8):
			tempCurrLoc = [currLoc[x] + 2**tempCurrLevel*offset[i][x] for x in range (3)]
			setIntermidiate (tempCurrLoc, [tempCurrLevel], scalarVal)
	if tempCurrLevel == 0:	
		for nodeNum in range (8):
			nodeLoc  = [currLoc[x] + offset[nodeNum][x] for x in range (3)]
			if scalarVal == 0:
				data[nodeLoc[0]][nodeLoc[1]][nodeLoc[2]] = 1
			else:
				data[nodeLoc[0]][nodeLoc[1]][nodeLoc[2]] = 0
		return 
    


#Function to read the bytes in to the variable dataBytes
def readBytes(fileName, dataBytes):
    with open(fileName, "rb") as f:
        byte = f.read(1)
        while byte != "":
            dataBytes.append(byte)
            byte = f.read(1)
    

#Convert the bytes in dataBytes into data
def convert(Datatyp, pointerLocInDataBytes, dataBytes, num): 
    s = pointerLocInDataBytes[0]
    pointerLocInDataBytes[0] = pointerLocInDataBytes[0] + num
    #return   struct.unpack("<"+Datatyp, ''.join(dataBytes[s:s+num]))[0]
    return   struct.unpack(Datatyp, ''.join(dataBytes[s:s+num]))[0]

# call the main function
if __name__ == "__main__":
    main()

