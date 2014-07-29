#coding=gb2312
import os
from datetime import datetime

#def gettimefromstring (timestr, formatstr='%Y-%m-%d-%H-%M-%S-%f'):
#    return datetime.strptime(timestr,formatstr)

def difftime (t1,t2):
    d1 = datetime.strptime(t1,'%Y-%m-%d-%H-%M-%S-%f')
    d2 = datetime.strptime(t2,'%Y-%m-%d-%H-%M-%S-%f')
    c = d2 - d1
    return c.total_seconds()

def recordFromMouseLog (logDir):
    mf = open(logDir + 'mouse.txt')
    mr = []

    countMouseLD = 0
    countLDImg = 0
    countLDAcc = 0
    countMouseMove = 0
    clickAccessInfo = {}
    while True:
        line = mf.readline()
        if line is None or line == '':
            break
        
        timestamp = line.strip()
        #print timestamp

        line = mf.readline().strip()
        if line == 'NOT LOGGED APPLICATION':
            mr.append((timestamp,line))
            continue
        
        #print line
        sarray = line.split(' ')
        lenOfArray = len(sarray)
        mousePosition = (int(sarray[-2]), int(sarray[-1]))
        mouseEvent = ' '.join(sarray[0:lenOfArray-2])
        
        if mouseEvent == 'MOUSE LEFT DOWN':
            countMouseLD = countMouseLD + 1

            imgFileName = logDir + 'screen/' + timestamp + '.png'
            if os.path.exists(imgFileName):
                countLDImg = countLDImg + 1
            else:
                print 'img: ', timestamp, ' doesn''t exist'
            
            accFileName = logDir + 'detail/' + timestamp + '.txt'
            if os.path.exists(accFileName):
                countLDAcc = countLDAcc + 1
                f = open(accFileName)
                ls = f.readlines()
                uitype = ls[2][6:].strip()
                uiname = ls[3][6:].strip()
                uivalue = ls[5][6:].strip()
                clickAccessInfo[timestamp] = (uitype,uiname,uivalue)
                #print ls[2],ls[3],ls[5]
            else:
                print 'acc: ' + timestamp, ' doesn''t exist'
        elif mouseEvent == 'MOUSE MOVE':
            countMouseMove = countMouseMove + 1
        
        line = mf.readline()

        idx = line.find(',')
        windowHwnd = line[0:idx]
        idx2 = line.rfind('(')
        idx3 = line.rfind(')')
        strRect = line[idx2+1:idx3]
        sarray = strRect.split(' ')
        windowRect = (int(sarray[0]),int(sarray[1]),int(sarray[2]),int(sarray[3]))

        windowName = line[idx+1 : idx2-1]


        line = mf.readline()
        idx = line.rfind(' ')
        processName = line[0:idx]
        processId = line[idx+1:]

        line = mf.readline()
        parentWin = line.strip()
        
        mr.append((timestamp,mouseEvent,mousePosition,windowName,windowRect,windowHwnd,processName,processId,parentWin))
    
    print difftime(mr[0][0],mr[-1][0]) / 60
    print countMouseLD, countLDImg, countLDAcc
    print countMouseMove

    return mr, clickAccessInfo

def lowlevelStat (mr):
    i = 1
    moveTotalTime = 0
    moveXDist = 0
    moveYDist = 0
    webui = {}
    while i<len(mr)-1:
        if mr[i][1] == 'NOT LOGGED APPLICATION':
            i = i+1
            continue

        if mr[i][1] == 'MOUSE LEFT DOWN':
            j = i+1
            while j<len(mr)-1 and mr[j][1] != 'MOUSE LEFT UP':
                j = j+1
            
            if mr[i][6] == 'iexplore.exe' or mr[i][6] == 'firefox.exe':
                try:
                    key = '#'.join(clickAccessInfo[mr[i][0]])
                    #key = clickAccessInfo[mr[i][0]][0] + '#' + clickAccessInfo[mr[i][0]][1]
                    if webui.has_key(key):
                        webui[key] = webui[key] + 1
                    else:
                        webui[key] = 1
                except Exception,e:
                    pass
            dt = difftime(mr[i][0],mr[j][0])
            dx = abs(mr[i][2][0] - mr[j][2][0])
            dy = abs(mr[i][2][1] - mr[j][2][1])
            if dx ==0 and dy == 0:
                pass#print 'MOUSE CLICK',dt,dx,dy
            else:
                pass#print 'MOUSE SELECTION',dt,dx,dy
            i = j
        elif mr[i][1] == 'MOUSE WHEEL':
            #print i,mr[i][0]
            j = i + 1
            while j<len(mr)-1 and mr[j][1] == 'MOUSE WHEEL':
                j = j+1
            
            try:
                dt = difftime(mr[i][0],mr[j-1][0])
            
                dx = abs(mr[i][2][0] - mr[j-1][2][0])
                dy = abs(mr[i][2][1] - mr[j-1][2][1])
                if j-i != 1:
                    pass#print i,j, j-i, dt, dx,dy, (j-i)/dt
            except Exception,e:
                pass

            i = j
        elif mr[i][1] == 'MOUSE MOVE':
            try:

                dt = difftime(mr[i-1][0],mr[i][0])
                dx = abs(mr[i-1][2][0] - mr[i][2][0])
                dy = abs(mr[i-1][2][1] - mr[i][2][1])
                #print dt, dx/dt, dy/dt

                moveTotalTime = moveTotalTime + dt
                moveXDist = moveXDist + dx
                moveYDist = moveYDist + dy

            except Exception,e:
                pass

        i = i + 1
    
    for u in webui:
        print u,webui[u]

    print moveTotalTime, moveXDist, moveYDist
    print moveXDist/moveTotalTime, moveYDist/moveTotalTime

def splitRecords (rs,maxtime): #maxtime: max afk time in seconds
    idxarr = [0]
    for i in range(len(mr)-1):
        dt = difftime(mr[i][0],mr[i+1][0])
        if dt > maxtime:
            idxarr.append(i)
            print mr[i][0],mr[i+1][0]
    
    idxarr.append(len(rs))
    return idxarr

def processStat (mr):
    si = 0
    ei = si + 1
    pr = []
    nolog = []
    while ei<len(mr):
        if mr[si][1] == 'NOT LOGGED APPLICATION':
            nolog.append(si)
            si = si + 1
            ei = si + 1
            continue

        if mr[ei][1] == 'NOT LOGGED APPLICATION' or mr[si][6] != mr[ei][6] :
            pr.append((mr[si][6],si,ei-1))
        
            si = ei
        ei = ei + 1
    

    print nolog
    print len(nolog),len(pr)
    
    pset = set()
    pstat = {}
    for p in pr:
        si = p[1]
        ei = p[2]
        if pstat.has_key(p[0]):
            pstat[p[0]] = pstat[p[0]] + difftime(mr[si][0],mr[ei][0])
        else:
            pstat[p[0]] = difftime(mr[si][0],mr[ei][0])
    
    print pstat
    
    return pr

def browserStat (pr, mr):
    chromestat = {}
    iestat = {}
    firefoxstat = {}
    for p in pr:
        if p[0] == 'chrome.exe':
            for i in range(p[1],p[2]+1):
                key = mr[i][3]
                if key == 'Chrome Legacy Window':
                    key = mr[i][-1]

                if chromestat.has_key(key):
                    chromestat[key] = chromestat[key] + 1
                else:
                    chromestat[key] = 1
        if p[0] == 'iexplore.exe':
            for i in range(p[1],p[2]+1):
                key = mr[i][3]
                if key.find('Internet Explorer') < 0:
                    key = mr[i][-1]

                if iestat.has_key(key):
                    iestat[key] = iestat[key] + 1
                else:
                    iestat[key] = 1
        if p[0] == 'firefox.exe':
            for i in range(p[1],p[2]+1):
                key = mr[i][3]
                if key.find('Mozilla Firefox') < 0:
                    key = mr[i][-1]

                if firefoxstat.has_key(key):
                    firefoxstat[key] = firefoxstat[key] + 1
                else:
                    firefoxstat[key] = 1

    for u in firefoxstat:
        print u,firefoxstat[u]
    
    for u in iestat:
        print u,iestat[u]

def getWindowTitle (r, processName):
    if processName == 'firefox.exe':
        t = r[3]
        if t.find('Mozilla Firefox') < 0:
            t = r[-1]
    
        return t
    elif processName == 'iexplore.exe':
        t = r[3]
        if t.find('Internet Explorer') < 0:
            t = r[-1]
    
        return t
    elif processName == 'chrome.exe':
        t = r[3]
        if t == 'Chrome Legacy Window':
            t = r[-1]
    
        return t
    else:
        return r[3]

def windowStat (mr, pr, processName):
    rs = []
    for p in pr:
        if p[0] != processName:
            continue
        
        si = p[1]
        ei = si + 1
        t1 = getWindowTitle(mr[si],processName)
        while ei<=p[2]:
            t2 = getWindowTitle(mr[ei],processName)
            if t1 != t2:
                rs.append((t1,si,ei-1))
                si = ei
                t1 = t2

            ei = ei + 1
     
        rs.append((t1,si,ei-1))
     
    return rs

if __name__ == '__main__':
    logDir = 'C:\\Users\\LingFeng\\Desktop\\hengtian\\log/'

    mr,clickAccessInfo = recordFromMouseLog(logDir)
    
    #lowlevelStat(mr)
    
    pr = processStat(mr)
    for p in pr:
        print p

    #browserStat (pr, mr)
    
    ffrs = windowStat(mr, pr , 'firefox.exe')
    iers = windowStat(mr, pr , 'iexplore.exe')
    chrs = windowStat(mr, pr , 'chrome.exe')
    #for r in ffrs:
    #    print r[0],r[1],r[2]
    #for r in chrs:
    #    print r[0],r[1],r[2]
    
    f = open('firefox.txt','w')
    webmodule = ['票据管理','电查管理','划款管理','配置管理','系统管理']
    for r in mr:
        if r[1] == 'NOT LOGGED APPLICATION':
            continue
        
        if r[6] == 'firefox.exe' and clickAccessInfo.has_key(r[0]):
            #f.write(' '.join([r[0],clickAccessInfo[r[0]][0],clickAccessInfo[r[0]][1],clickAccessInfo[r[0]][2]]))
            #f.write('\n')
            
            #if clickAccessInfo[r[0]][1] in webmodule:
            #    print r[0],clickAccessInfo[r[0]][0],clickAccessInfo[r[0]][1],clickAccessInfo[r[0]][2]
            
            #if clickAccessInfo[r[0]][2].find('http')>=0 or clickAccessInfo[r[0]][1] in webmodule:
            #    f.write(' '.join([r[0],clickAccessInfo[r[0]][0],clickAccessInfo[r[0]][1],clickAccessInfo[r[0]][2]]))
            #    f.write('\n')
            #    print r[0],clickAccessInfo[r[0]][0],clickAccessInfo[r[0]][1],clickAccessInfo[r[0]][2]
            

            if clickAccessInfo[r[0]][0] == '按钮' or clickAccessInfo[r[0]][0] == '超级连接':
                f.write(' '.join([r[0],clickAccessInfo[r[0]][0],clickAccessInfo[r[0]][1],clickAccessInfo[r[0]][2]]))
                f.write('\n')
                print r[0],clickAccessInfo[r[0]][0],clickAccessInfo[r[0]][1],clickAccessInfo[r[0]][2]
            elif clickAccessInfo[r[0]][1].strip() != '' and clickAccessInfo[r[0]][2].strip() != '':
                f.write(' '.join([r[0],clickAccessInfo[r[0]][0],clickAccessInfo[r[0]][1],clickAccessInfo[r[0]][2]]))
                f.write('\n')
                print r[0],clickAccessInfo[r[0]][0],clickAccessInfo[r[0]][1],clickAccessInfo[r[0]][2]
            else:
                pass#print clickAccessInfo[r[0]][0],clickAccessInfo[r[0]][1],clickAccessInfo[r[0]][2]
        
    f.close() 
    

 
