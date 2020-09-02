import QtQuick 2.0
import Spine 1.0

Item{
    id: root
    //property bool debugTracer: false

    property alias skeletonDataFile: skeleton.skeletonDataFile
    property alias atlasFile: skeleton.atlasFile
    property alias scale: skeleton.scale
    property alias skin: skeleton.skin
    property alias timeScale: skeleton.timeScale
    property alias premultipliedAlapha: skeleton.premultipliedAlapha
    property alias debugSlots: skeleton.debugSlots
    property alias debugBones: skeleton.debugBones
    property alias sourceSize: skeleton.sourceSize

    property real aspect: sourceSize.height/sourceSize.width

    signal start(int trackIndex)
    signal end(int trackIndex, string termname)
    signal complete(int trackIndex, int loopCount, string termname)
    signal event(int trackIndex, SpineEvent event)

    function setToSetupPose(){
        skeleton.setToSetupPose();
    }
    function setBonesToSetupPose(){
        skeleton.setBonesToSetupPose();
    }
    function setSlotsToSetupPose(){
        skeleton.setSlotsToSetupPose();
    }
    function setAttachment(slotName, attachmentName){
        skeleton.setAttachment(slotName, attachmentName);
    }
    function setMix(fromAnimation, toAnimation, duration){
        skeleton.setMix(fromAnimation, toAnimation, duration);
    }
    function setAnimation (trackIndex, name, loop){
        skeleton.setAnimation(trackIndex, name, loop);
    }
    function addAnimation (trackIndex, name, loop, delay){
        skeleton.addAnimation(trackIndex, name, loop, delay === null?0:delay);
    }
    function clearTracks(){
        skeleton.clearTracks();
    }
    function clearTrack(trackIndex){
        skeleton.clearTrack(trackIndex===null?0:trackIndex);
    }

    // EXTRA
    function setTrackDataTerm(ti, termname)
    {
        console.log("ANIMATION set term", termname)
        skeleton.setTrackDataTerm(ti, termname)
    }

    SkeletonAnimationFbo
    {
        id: skeleton

        width: parent.width
        height: Math.min(parent.width*aspect, parent.height)

        /*
        Timer
        {
            interval: 10
            repeat: true
            running: true
            onTriggered: skeleton.updateSkeletonAnimation();
        }
        */

        property int ticks: 0

        NumberAnimation on ticks {
            from: 0; to: 100;
            loops: Animation.Infinite
            running: true
        }

        onTicksChanged: skeleton.updateSkeletonAnimation();


        onSkeletonStart:{
            root.start(trackIndex);
        }
        onSkeletonEnd:{
            root.end(trackIndex, termname);
        }
        onSkeletonComplete:{
            root.complete(trackIndex, loopCount, termname);
        }
        onSkeletonEvent:{
            root.event(trackIndex, event);
        }
    }

    /*
    Rectangle{
        visible: root.debugTracer
        anchors.centerIn: parent
        width: 8; height:8
        color: "#60ff0000"
    }

    Rectangle{
        visible: root.debugTracer
        anchors.fill: skeleton
        color:"transparent"
        border.width: 1
        border.color: "black"
    }
    */
}
