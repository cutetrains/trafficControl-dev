import sys
import re
from operator import itemgetter
from math import radians, sin, cos, sqrt, asin, atan

def haversine(lon1, lat1, lon2, lat2):  #From rosettacode.org
 
  R = 6372.8 # Earth radius in kilometers
  dLat = radians(lat2 - lat1)
  dLon = radians(lon2 - lon1)
  lat1 = radians(lat1)
  lat2 = radians(lat2)
 
  a = sin(dLat/2)**2 + cos(lat1)*cos(lat2)*sin(dLon/2)**2
  c = 2*asin(sqrt(a))
 
  return R * c

#print( 'Number of arguments:', len(sys.argv), 'arguments.')
#print( 'Argument List:', str(sys.argv))

if(len(sys.argv) != 2):
  print("ERROR! KML file not specified")
else:
  #print("OK")
  inputfile = str(sys.argv[1])

  trackList=[] # The track list contains name, start coordinates, start station name, end coordinates, 
               # end station name, length
  trackListItem=["","","","",""]
  stationList=[] # The station list contains name, the coordinates, and (S) or (J)
  stationListItem=["","","",""]

  file = ""
  with open(inputfile, "r", encoding='utf8') as ins:
    for line in ins:
      file += line
    placemarkList=re.findall('<Placemark>(.*?)<\/Placemark>', file, re.MULTILINE| re.DOTALL)
    for pm in placemarkList:
      nameList = re.findall('<name>(.*?)<\/name>', pm)
      thisName=nameList[0].replace("-","_")
      thisCoordinate = re.findall('<coordinates>\n*\t*(.*?)\n*\t*<\/coordinates>', pm, re.MULTILINE| re.DOTALL)
      thisIsVisible = re.findall('<visibility>(.*?)<\/visibility>', pm, re.MULTILINE| re.DOTALL)
      if(len(thisIsVisible) ==0):
        coordinateList=thisCoordinate[0].split(" ")
        coordinateList = [x[:-2] for x in coordinateList]
        #SWAP LAT, LON
        swappedCoordinateList = []
        for x in coordinateList:
          xtemp=x.split(',')
          if(len(xtemp)>1):#EMPTY LINE SOMETIMES
            swappedCoordinateList.append(xtemp[1]+" " + xtemp[0])
            
        if(len(swappedCoordinateList) > 1 ):
          #TRACK
          cSum = 0
          for iii in range(1, len(swappedCoordinateList)):
            orgLat=swappedCoordinateList[iii-1].split(" ")[0]
            orgLon=swappedCoordinateList[iii-1].split(" ")[1]
            desLat=swappedCoordinateList[iii].split(" ")[0]
            desLon=swappedCoordinateList[iii].split(" ")[1]
            cSum=cSum+haversine(float(orgLon),float(orgLat),float(desLon),float(desLat))

          #FINAL WHITESPACE CONFUSES THE SPLIT METHOD FOR POLYLINES
          #TODO ADD TRACKS TO INTERNAL LIST OF TRACKS
          if(thisName[0]=="d"):
            #FIND THE DIRECTION OF THE TRACK
            startLat = float(swappedCoordinateList[0].split(" ")[0])
            startLong = float(swappedCoordinateList[0].split(" ")[1])
            endLat = float(swappedCoordinateList[-1].split(" ")[0])
            endLong = float(swappedCoordinateList[-1].split(" ")[1])
            lonShift = -0.0005*(endLat-startLat)/(sqrt((endLong-startLong)**2+(endLat-startLat)**2))
            latShift = 0.0005*(endLong-startLong)/(sqrt((endLong-startLong)**2+(endLat-startLat)**2))
            #print("      Shift: Long: " + str(lonShift ) + " Lat: " + str(latShift )+ " Angle: " + str(180/3.14*atan(latShift/lonShift)))
            if (abs(endLat - startLat) < abs(endLong - startLong)):
              if(startLong<endLong):
                #print("E")
                reversedName = thisName+ "_W"
                thisName = thisName+"_E"
              else:
                #print("W")
                reversedName = thisName+ "_E"
                thisName = thisName+"_W"
            else:
              if(startLat<endLat):
                #print("N")
                reversedName = thisName+ "_S"
                thisName = thisName+"_N"
              else:
                #print("S")
                reversedName = thisName+ "_N"
                thisName = thisName+"_S"
            reversedCoordinates=[]
            for iii in range(1, len(coordinateList)):
              orgLon=coordinateList[iii-1].split(",")[0]
              orgLat=coordinateList[iii-1].split(",")[1]
              orgLonR = str(float(orgLon) + lonShift)
              orgLatR = str(float(orgLat) + latShift)
              reversedCoordinates.insert(0,orgLatR + " " + orgLonR)

            trackListItem = [reversedName, reversedCoordinates[0].split(' '), "", reversedCoordinates[-1].split(' '), "", cSum]
            print("ADD TRACK " + reversedName +" "+ str(int(cSum*1000))+ " COORDINATES " + ' '.join(reversedCoordinates))
            trackList.append(trackListItem)
          trackListItem = [thisName, swappedCoordinateList[0].split(' '), "", swappedCoordinateList[-1].split(' '), "", cSum]  
          #print(trackListItem)
          print("ADD TRACK " + thisName +" "+ str(int(cSum*1000))+ " COORDINATES " + ' '.join(swappedCoordinateList))
          trackList.append(trackListItem)
        else:
          #TRACK
          if(( thisName[0]=="J" ) and (thisName[1].isupper() ) ) :
            junctionText=" AS JUNCTION"
          else:
            junctionText=""
          print("ADD STATION " + thisName + junctionText + " COORDINATES " + ' '.join(swappedCoordinateList))
          stationListItem = [thisName, swappedCoordinateList[0].split(' ')]
          stationList.append(stationListItem)#TODO: ADD INFO ABOUT HOW MANY TRACKS TO USE IN TRACK NAME

    for track in trackList: #FIND START AND END STATION FOR TRACK
      maxAllowedDist=200
      startDist=100000000.0
      startCandidate=""
      startCoordinate=[]
      endDist=100000000.0
      endCandidate=""
      endCoordinate=[]
      for station in stationList:
        thisDist=haversine(float(track[1][0]), float(track[1][1]) ,float(station[1][0]), float(station[1][1] ))
        if(thisDist<startDist):
          startDist=thisDist
          startCandidate=station[0]
          startCoordinate=station[1]
        thisDist=haversine(float(track[3][0]), float(track[3][1]) ,float(station[1][0]), float(station[1][1] ))
        if(thisDist<endDist):
          endDist=thisDist
          endCandidate=station[0]
          endCoordinate=station[1]
      trackDir=( "__N" ) if ( float(startCoordinate[1] )<  float(endCoordinate[1]) ) else ( "__S" )  
      trackDir=trackDir +("__E" ) if ( float(startCoordinate[0] )<  float(endCoordinate[0]) ) else (trackDir + "__W" )
      if ((startDist<maxAllowedDist) and (endDist<maxAllowedDist) and (startCandidate != endCandidate)):
        if(track[0][-1] in trackDir):
          print("CONNECT TRACK " + track[0] + " FROM " + startCandidate + " TO "+ endCandidate)
          print("CONNECT TRACK " + track[0] + " FROM " + endCandidate + " TO "+ startCandidate)
        else:
          print("CONNECT TRACK " + track[0] + " FROM " + endCandidate + " TO " + startCandidate)
          print("CONNECT TRACK " + track[0] + " FROM " + startCandidate + " TO " + endCandidate)
      else:
        print("   ERROR: missing station at start or end for track: " + track[0] +" "+ str(startDist)+" " + str(endDist)+" " + startCandidate +" "+ endCandidate )    
