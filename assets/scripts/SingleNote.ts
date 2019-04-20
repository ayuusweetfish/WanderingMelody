// Learn TypeScript:
//  - [Chinese] https://docs.cocos.com/creator/manual/zh/scripting/typescript.html
//  - [English] http://www.cocos2d-x.org/docs/creator/manual/en/scripting/typescript.html
// Learn Attribute:
//  - [Chinese] https://docs.cocos.com/creator/manual/zh/scripting/reference/attributes.html
//  - [English] http://www.cocos2d-x.org/docs/creator/manual/en/scripting/reference/attributes.html
// Learn life-cycle callbacks:
//  - [Chinese] https://docs.cocos.com/creator/manual/zh/scripting/life-cycle-callbacks.html
//  - [English] http://www.cocos2d-x.org/docs/creator/manual/en/scripting/life-cycle-callbacks.html

const { ccclass, property } = cc._decorator;

@ccclass
export default class SingleNote extends cc.Component {

    @property(cc.Label)
    label: cc.Label = null;

    @property
    text: string = 'hello';

    sound: string;

    // LIFE-CYCLE CALLBACKS:

    initSound(url){
        this.sound = url;
    }

    getY() {
        return this.node.convertToWorldSpace(cc.v2(0, 0)).y;
    }
    
    checkPlay(){
        var dist = 80 - this.getY();

        if(Math.abs(dist) <250){
            cc.loader.loadRes(this.sound, cc.AudioClip, function(err, soundClip){
                if(err){
                    cc.error("Error in noteplay");
                }
                cc.audioEngine.playEffect(soundClip, false);
            });
            this.node.getParent().getComponent("Track").receiveDelta(dist);
            this.node.color = cc.color(128, 128, 255);
        }
    }
    // onLoad () {}

    start() {

    }

    // update (dt) {}
}
