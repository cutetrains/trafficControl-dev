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

inputfile = "ActiveTrainNetwork_DoesjebroMissing.kml"

trackList=[] # The track list contains name, line in file, start coordinates, start station name, end coordinates, 
             # end station name, length
trackListItem=["","","","","",""]
stationList=[] # The station list contains name, line in file, the coordinates, and (S) or (J)
stationListItem=["","","",""]
thisTrackCoordinates=""
ignorePM = 0

with open(inputfile, "r", encoding='utf8') as ins:
  maxAllowedDist=200
  numLines=0
  placeMarkType=""
  placeMarkName=""
  isNextLineCoordinateList=False
  for line in ins:
    numLines += 1

    if ( ("\t\t\t<name>" in line) and not (("\t\t\t\t<name>") in line) ) :
      placeMarkType=line[9:-8]
      ignorePM = 0
    if ( ("\t\t\t\t<name>" in line) and not (("\t\t\t\t\t<name>") in line) ) :
      #placeMarkName=line[10:-8].replace("Å","Aa").replace("Ä","Ae").replace("Ö","Oe").replace("å","aa").replace("ä","ae").replace("ö","oe")
      placeMarkName=line[10:-8].replace("-","_")
      ignorePM = 0
    if("visibility>0</visibility>" in line):
      ignorePM = 1

    ###########
    # STATION #
    ###########
    if ( ( "<coordinates>" in line) and ( ( "Station" in placeMarkType) or ("Junction" in placeMarkType ) ) ) :
      if(ignorePM == 0):
        if " " in placeMarkName:
          print("-"*2000)
        stationListItem=[placeMarkName, numLines, line[18:-17].split(","), ("Junction" in placeMarkType) and "(S)" or "(J)"]
        #print(stationListItem[2][1])
        stationList.append(stationListItem)
        print("ADD STATION " + placeMarkName + str(" AS JUNCTION" if ("Junction" in placeMarkType) else "") + " COORDINATES " + stationListItem[2][1] +" " +
            stationListItem[2][0] )
        
    #########################
    # COORDINATES FOR TRACK #
    #########################
    if(isNextLineCoordinateList==True):#This happens directly after coordinates are found below
      summa=0.0
      coordinateList=str(line[6:-2]).split(" ")
      thisTrackCoordinates=""
      reversedTrackCoordinates=""

      # IS DELTA north-south OR west-east?
      startLong = float(coordinateList[0].split(",")[0])
      startLat = float(coordinateList[0].split(",")[1])
      endLong = float(coordinateList[-1].split(",")[0])
      endLat = float(coordinateList[-1].split(",")[1])
      #print("      DeltaLon: "+ str(endLong-startLong) + " DeltaLat: "+ str(endLat-startLat) + " Angle: " + str(180/3.14*atan((endLat-startLat)/(endLong-startLong))))
      lonShift = -0.0005*(endLat-startLat)/(sqrt((endLong-startLong)**2+(endLat-startLat)**2))
      latShift = 0.0005*(endLong-startLong)/(sqrt((endLong-startLong)**2+(endLat-startLat)**2))
      #print("      Shift: Long: " + str(lonShift ) + " Lat: " + str(latShift )+ " Angle: " + str(180/3.14*atan(latShift/lonShift)))
      if (abs(endLat - startLat) < abs(endLong - startLong)):
        if(startLong<endLong):
          #print("E")
          placeMarkName = placeMarkName+"_E"
        else:
          #print("W")
          placeMarkName = placeMarkName+"_W"
      else:
        if(startLat<endLat):
          #print("N")
          placeMarkName = placeMarkName+"_N"
        else:
          #print("S")
          placeMarkName = placeMarkName+"_S"
      
      for iii in range(1, len(coordinateList)):
        orgLon=coordinateList[iii-1].split(",")[0]
        orgLat=coordinateList[iii-1].split(",")[1]
        desLon=coordinateList[iii].split(",")[0]
        desLat=coordinateList[iii].split(",")[1]
        summa=summa+haversine(float(orgLon),float(orgLat),float(desLon),float(desLat))
        
        if(placeMarkName[0]=="d"):
          orgLonR = str(float(orgLon) + lonShift)
          orgLatR = str(float(orgLat) + latShift)
          reversedTrackCoordinates=orgLatR + " " + orgLonR + " " + reversedTrackCoordinates
        thisTrackCoordinates=thisTrackCoordinates+" " + orgLat + " " + orgLon
      if(placeMarkName[0]=="d"):
        desLonR = str(float(desLon) + lonShift)
        desLatR = str(float(desLat) + latShift)   
        reversedTrackCoordinates=  desLatR + " " + desLonR + " " + reversedTrackCoordinates 
      thisTrackCoordinates=thisTrackCoordinates+" " + desLat + " " + desLon

      isNextLineCoordinateList=False
      trackListItem=[placeMarkName, numLines, coordinateList[0].split(",")[:-1], "", coordinateList[-1].split(",")[:-1], "", summa]
      trackList.append(trackListItem)
      
      print("ADD TRACK "+ placeMarkName + " " + str(int(summa*1000) ) +  " COORDINATES" +thisTrackCoordinates)
      if(placeMarkName[0]=="d"):#dLunGunN shall have a friend dLunGunS (same name, but in opposite direction)
        tempPlaceMarkName=""
        if (placeMarkName[-1]=='N'): tempPlaceMarkName='S'
        if (placeMarkName[-1]=='S'): tempPlaceMarkName='N'
        if (placeMarkName[-1]=='E'): tempPlaceMarkName='W'
        if (placeMarkName[-1]=='W'): tempPlaceMarkName='E'
        print("ADD TRACK " + str(placeMarkName[:-1])+tempPlaceMarkName + " " + str(int(summa*1000))  +  " COORDINATES " +reversedTrackCoordinates[:-1])
        trackListItem=[str(placeMarkName[:-1])+tempPlaceMarkName, numLines, coordinateList[0].split(",")[:-1], "", coordinateList[-1].split(",")[:-1], "", summa]
        trackList.append(trackListItem)

    #########
    # TRACK #
    #########
    if ( ( "<coordinates>" in line) and ( "Track" in placeMarkType ) ):
      if(ignorePM == 0):
        isNextLineCoordinateList=True

  for track in trackList: #FIND START AND END STATION FOR TRACK
    startDist=100000000.0
    startCandidate=""
    startCoordinate=[]
    endDist=100000000.0
    endCandidate=""
    endCoordinate=[]
    for station in stationList:
      thisDist=haversine(float(track[2][0]), float(track[2][1]) ,float(station[2][0]), float(station[2][1] ))
      if(thisDist<startDist):
        startDist=thisDist
        startCandidate=station[0]
        startCoordinate=station[2]
      thisDist=haversine(float(track[4][0]), float(track[4][1]) ,float(station[2][0]), float(station[2][1] ))
      if(thisDist<endDist):
        endDist=thisDist
        endCandidate=station[0]
        endCoordinate=station[2]
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
      print("ERROR: missing station at start or end for track: " + track[0])
