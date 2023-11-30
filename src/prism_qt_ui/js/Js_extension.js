.pragma library
.import QtQuick 2.15   as QtQuick2
function prismTimer(parent) {
    return Qt.createQmlObject("import QtQuick 2.0; Timer {}", parent)
}

function delay(parent, delayTime, cb) {
    var timer = prismTimer(parent)
    timer.interval = delayTime
    timer.repeat = false
    timer.triggered.connect(cb)
    timer.start()
}
function setColorAlpha(color, alpha) {
    return Qt.hsla(color.hslHue, color.hslSaturation, color.hslLightness, alpha)
}
function getFont(size) {
    return Qt.font({
                       "pixelSize": size,
                       "family": "微软雅黑"
                   })
}

function expandTreeNode(t,ni) {
    //if(!t.isExpanded(ni))
        t.expand(ni)
    var cc = t.model.rowCount(ni)
    if(cc>0) {
        for(var i=0;i<cc;i++) {
            var nni = t.model.index(i,0,ni)
            //if(!t.isExpanded(nni))
            //这里不样全部展开了,节点太多卡卡的,只展开两层就将将好
            //expandTreeNode(t,nni)
            t.expand(nni)



//            var cc2 = t.model.rowCount(nni)
//            if(cc2>0) {
//                for(var i2=0;i2<cc2;i2++) {
//                    var nni2 = t.model.index(i2,0,nni)
//                    //if(!t.isExpanded(nni))
//                    t.expand(nni2)

//                }
//            }
        }
    }
}
function expandTree(t) {
    for(var i=0; i < t.model.rowCount(); i++) {
        var idx = t.model.index(i,0)
        expandTreeNode(t,idx);
    }
}

function collapseTreeNode(t,ni) {
    //if(t.isExpanded(ni))
        t.collapse(ni)
    var cc = t.model.rowCount(ni)
    if(cc>0) {
        for(var i=0;i<cc;i++) {
            var nni = t.model.index(i,0,ni)
            //if(t.isExpanded(nni))
                collapseTreeNode(t,nni)
        }
    }
}
function collapseTree(t) {
    for(var i=0; i < t.model.rowCount(); i++) {
        var idx = t.model.index(i,0)
        collapseTreeNode(t,idx);
    }
}
