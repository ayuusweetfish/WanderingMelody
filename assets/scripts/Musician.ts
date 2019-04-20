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
export default class Musician extends cc.Component {
    @property(cc.JsonAsset)
    private scoredata;
    @property(cc.Node)
    public trackA: cc.Node;
    @property(cc.Node)
    public trackB: cc.Node;
    @property
    public rateRank: number = 1;
    @property(cc.Label)
    public label: cc.Label;
    
    public timer: number = 0;
    public totalLength: number = 0;
    public rate: number = 0;
    public accel: number = 0;
    public accelRemaining: number = 0;
    public notesPosition=[];
    onLoad() {
        this.totalLength = this.scoredata.json.totalLength;
        this.rate = this.scoredata.json.initRate;
        
        cc.loader.loadResArray(this.scoredata.json.sounds, cc.AudioClip, function(err, soundslist){
            if (err){
                cc.error(err.message || err);
                return;
            }            
        });
        this.setNotesPosition(this.scoredata.json.tracks.trackA.notes);
        this.setNotesPosition(this.scoredata.json.tracks.trackB.notes);
        console.log(this.notesPosition);        
        this.trackA.getComponent('Track').addNotes(this.scoredata.json.tracks.trackA.notes, this.scoredata.json.sounds);
        this.trackB.getComponent('Track').addNotes(this.scoredata.json.tracks.trackB.notes, this.scoredata.json.sounds);
        this.timer = 0;
    }

    setNotesPosition(notesarr){
        for(var i in notesarr){
            this.notesPosition.push(notesarr[i].position);
        }
        this.notesPosition=Array.from(new Set(this.notesPosition));
        this.notesPosition.sort();        
    }

    // adjustSpeed (expectedDist, deltaDist) {
    //     var expectedTime = expectedDist / this.rate;
    //     var deltaTime = deltaDist / this.rate;
    //     var targetTime = (expectedDist + deltaDist) / expectedDist * (expectedTime + deltaTime);
    //     var expectedVel = expectedDist / (expectedTime + deltaTime);
    //     var targetAccel = (expectedVel - this.rate) / targetTime;
    //     this.accel = targetAccel;
    // }
    adjustSpeed (D, d) {
        d *= 0.5;
        const T_min = 0.4;
        const A_max = 100;
        var dV = (d / D) * this.rate;
        var A = dV / T_min;
        if (A > A_max) A = A_max; else if (A < -A_max) A = -A_max;
        this.accel = A;
        this.accelRemaining = dV / A;
        console.log(this.accel, this.accelRemaining);
    }

    update (dt) {
        this.node.y -= this.rateRank * dt * (this.rate + 0.5 * dt * this.accel);
        this.rate -= dt * this.accel;
        // TODO: Correctly handle cases where dt < accelRemaining
        if ((this.accelRemaining -= dt) <= 0) this.accel = 0;
        this.label.string = Math.round(this.rate);
    }
}
