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
export default class NewClass extends cc.Component {

    @property(cc.Prefab)
    singleNote: cc.Prefab;

    @property
    text: string = 'a';

    @property
    trackKey: number = 65;

    noteList =[];

    noteCursor: number = 0;
    // LIFE-CYCLE CALLBACKS:

    onLoad () {
        cc.systemEvent.on(cc.SystemEvent.EventType.KEY_DOWN, this.onKeyDown, this);
    }
    addNotes(notes, sounds){
        for(var ind in notes){
            this.spawnNewNote(notes[ind].position, sounds[notes[ind].sound]);
        }
        console.log(this.node.children);
    }

    receiveDelta(delta){
        this.noteCursor += 1;
        if (this.noteCursor < 2) return;
        var curNote = this.node.children[this.noteCursor - 1].getComponent("SingleNote");
        var lastNote = this.node.children[this.noteCursor - 2].getComponent("SingleNote");
        this.node.getParent().getComponent("Musician").adjustSpeed(
            curNote.getY() - lastNote.getY(), delta);
        console.log('delta = ', delta);
    }

    onKeyDown(event){
        if ((event.keyCode ==this.trackKey) && this.noteCursor<this.noteList.length){
            this.node.children[this.noteCursor].getComponent("SingleNote").checkPlay();
        }
    }

    
    

    spawnNewNote(position, soundurl){
        var newNote = cc.instantiate(this.singleNote);
        this.noteList.push(position);
        this.node.addChild(newNote);
        newNote.setPosition(cc.v2(0,50*position));
        newNote.getComponent('SingleNote').initSound(soundurl);
    }
    
    start() {

    }
    onDestroy(){
        cc.systemEvent.off(cc.SystemEvent.EventType.KEY_DOWN, this.onKeyDown, this);
    }

    // update (dt) {}
}
