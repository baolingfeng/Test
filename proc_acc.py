#coding=utf8
import proc

if __name__ == '__main__':
    logDir = 'log/'
    mr,clickAccessInfo = proc.recordFromMouseLog(logDir)

    for r in mr:
        if clickAccessInfo.has_key(r[0]) and r[6] == 'iexplore.exe':
            f = open('acc/' + r[0] + '.txt','w')
            f.write(clickAccessInfo[r[0]][0] + '\n')
            f.write(clickAccessInfo[r[0]][1] + '\n')
            f.write(clickAccessInfo[r[0]][2] + '\n')
            f.close()
        