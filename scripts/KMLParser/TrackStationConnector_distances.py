from operator import itemgetter
from math import radians, sin, cos, sqrt, asin

def haversine(lon1, lat1, lon2, lat2):  #From rosettacode.org
 
  R = 6372.8 # Earth radius in kilometers
  #R = 6372800 # Earth radius in kilometers
  dLat = radians(lat2 - lat1)
  dLon = radians(lon2 - lon1)
  lat1 = radians(lat1)
  lat2 = radians(lat2)
 
  a = sin(dLat/2)**2 + cos(lat1)*cos(lat2)*sin(dLon/2)**2
  c = 2*asin(sqrt(a))
 
  return R * c

inputfile = "NetworkCoordinates.kml"

trackList=[] # The track list contains name, line in file, start coordinates, start station name, end coordinates, 
             # end station name, length
trackListItem=["","","","","",""]
stationList=[] # The station list contains name, line in file, the coordinates, and (S) or (J)
#THIS REQUIRES UPDATE IN TRAINCONTROL::IMPORTNETWORKFROMFILE
stationListItem=["","","",""]
thisTrackCoordinates=""


with open(inputfile, "r", encoding='utf8') as ins:
  maxAllowedDist=200
  numLines=0
  lineArray=[]
  placeMarkType=""
  placeMarkName=""
  isNextLineCoordinateList=False
  for line in ins:
    numLines += 1
    lineArray.append(line)

    if ( ("\t\t\t<name>" in line) and not (("\t\t\t\t<name>") in line) ) :
      placeMarkType=line[9:-8]
    if ( ("\t\t\t\t<name>" in line) and not (("\t\t\t\t\t<name>") in line) ) :
      placeMarkName=line[10:-8]
    if ( ( "<coordinates>" in line) and ( ( "Station" in placeMarkType) or ("Junction" in placeMarkType ) ) ) :
      #print("ADD STATION " + placeMarkName + str(" AS JUNCTION" if ("Junction" in placeMarkType) else "") )
      if " " in placeMarkName:
        print("-"*2000)
      stationListItem=[placeMarkName, numLines, line[18:-17].split(","), ("Junction" in placeMarkType) and "(S)" or "(J)"]
      #print(stationListItem[2][1])
      stationList.append(stationListItem)
      print("ADD STATION " + placeMarkName + " COORDINATES " + stationListItem[2][1] +" " +
            stationListItem[2][0] + str(" AS JUNCTION" if ("Junction" in placeMarkType) else "") )

    if(isNextLineCoordinateList==True):#This happens directly after coordinates are found below
      summa=0.0
      tempDist=0.0
      coordinateList=str(line[6:-2]).split(" ")
      #print(coordinateList)
      thisTrackCoordinates=""
      reversedTrackCoordinates=""
      for iii in range(1, len(coordinateList)):
        orgLon=coordinateList[iii-1].split(",")[0]
        orgLat=coordinateList[iii-1].split(",")[1]
        desLon=coordinateList[iii].split(",")[0]
        desLat=coordinateList[iii].split(",")[1]
        tempDist=haversine(float(orgLon),float(orgLat),float(desLon),float(desLat))
        summa=summa+tempDist
        if(placeMarkName[0]=="d"):
          reversedTrackCoordinates=orgLat + " " + orgLon + " " + reversedTrackCoordinates
        thisTrackCoordinates=thisTrackCoordinates+" " + orgLat + " " + orgLon
      if(placeMarkName[0]=="d"):
        reversedTrackCoordinates=  desLat + " " + desLon + " " + reversedTrackCoordinates 
      #Modify here to add temp distances
      thisTrackCoordinates=thisTrackCoordinates+" " + desLat + " " + desLon
      #print(thisTrackCoordinates)
      isNextLineCoordinateList=False
      trackListItem=[placeMarkName, numLines, coordinateList[0].split(",")[:-1], "", coordinateList[-1].split(",")[:-1], "", summa]
      trackList.append(trackListItem)
      
      print("ADD TRACK "+ placeMarkName + " " + str(int(summa*1000) ) +  " COORDINATES" +thisTrackCoordinates )
      if(placeMarkName[0]=="d"):#dLunGunN shall have a friend dLunGunS (same name, but in opposite direction)
        tempPlaceMarkName=""
        if (placeMarkName[-1]=='N'): tempPlaceMarkName='S'
        if (placeMarkName[-1]=='S'): tempPlaceMarkName='N'
        if (placeMarkName[-1]=='E'): tempPlaceMarkName='W'
        if (placeMarkName[-1]=='W'): tempPlaceMarkName='E'
        print("ADD TRACK " + str(placeMarkName[:-1])+tempPlaceMarkName + " " + str(int(summa*1000))  +  " COORDINATES" +reversedTrackCoordinates)
        trackListItem=[str(placeMarkName[:-1])+tempPlaceMarkName, numLines, coordinateList[0].split(",")[:-1], "", coordinateList[-1].split(",")[:-1], "", summa]
        trackList.append(trackListItem)

    if ( ( "<coordinates>" in line) and ( "Track" in placeMarkType ) ):
      isNextLineCoordinateList=True

  for track in trackList:
    firstDist=100000000.0
    firstCandidate=""
    firstCoordinate=[]
    lastDist=100000000.0
    lastCandidate=""
    lastCoordinate=[]
    for station in stationList:
      thisDist=haversine(float(track[2][0]), float(track[2][1]) ,float(station[2][0]), float(station[2][1] ))
      if(thisDist<firstDist):
        firstDist=thisDist
        firstCandidate=station[0]
        firstCoordinate=station[2]
      thisDist=haversine(float(track[4][0]), float(track[4][1]) ,float(station[2][0]), float(station[2][1] ))
      if(thisDist<lastDist):
        lastDist=thisDist
        lastCandidate=station[0]
        lastCoordinate=station[2]
    trackDir=( "N" ) if ( float(firstCoordinate[1] )<  float(lastCoordinate[1]) ) else ( "S" )
    trackDir=trackDir+ ( "E" ) if ( float(firstCoordinate[0] )<  float(lastCoordinate[0]) ) else (trackDir + "W" ) 
    if(track[0][-1] in trackDir):
      print("CONNECT TRACK " + track[0] + " FROM " + firstCandidate + " TO "+ lastCandidate + " DEFAULT")
      #THIS REQUIRES UPDATE IN TRAINCONTROL::IMPORTNETWORKFROMFILE
      print("CONNECT TRACK " + track[0] + " FROM " + lastCandidate + " TO "+ firstCandidate)
    else:
      print("CONNECT TRACK " + track[0] + " FROM " + lastCandidate + " TO " + firstCandidate + " DEFAULT")
      print("CONNECT TRACK " + track[0] + " FROM " + firstCandidate + " TO " + lastCandidate)
