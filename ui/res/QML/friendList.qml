import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {

    //传递出去当前是哪个id对应的对话被点击了，找出对应的聊天记录
    signal signalFriendListClicked(string strId,string strName);

    //增加元素到模型中，也就是增加一个会话到好友列表之中
    function addElementToModel(strImagePath,strName,strId,strFirstChacter)
    {
        addressBookModel.append({"imagePath":strImagePath,"name":strName,"strIdx":strId,"firstChacter":strFirstChacter});
    }

    //清空好友列表
    function clearModel()
    {
        addressBookModel.clear();
    }

    //根据id找到这个好友所在模型中的位置
    function findPosInModel(iId)
    {
        for(var i=0;i<addressBookModel.count;++i)
        {
            if(addressBookModel.get(i).id===iId)
            {
                moveActiveFriend2Top(i);
                break;
            }
        }
    }

    //把这个会话移动到列表最顶端，当有新对话来时使用
    function moveActiveFriend2Top(iPos)
    {
        addressBookModel.move(iPos,0,1);
    }

    //通讯录窗口
    id: addressBook;

    width: 200
    height: 100
    color: "transparent";

    ListModel
    {
        id: addressBookModel;
        /*ListElement
        {
            imagePath:"strImagePath"
            name:"strName"
            idx:"1"
            firstChacter:"A"
        }
        ListElement
        {
            imagePath:"strImagePath"
            name:"strName"
            idx:"1"
            firstChacter:"A"
        }
        ListElement
        {
            imagePath:"strImagePath"
            name:"strName"
            idx:"1"
            firstChacter:"A"
        }
        ListElement
        {
            imagePath:"strImagePath"
            name:"strName"
            idx:"1"
            firstChacter:"B"
        }
        ListElement
        {
            imagePath:"strImagePath"
            name:"strName"
            idx:"1"
            firstChacter:"C"
        }
        ListElement
        {
            imagePath:"strImagePath"
            name:"strName"
            idx:"1"
            firstChacter:"群聊"
        }*/
    }

    ListView
    {
        id:addressBookView;
        model: addressBookModel;
        delegate: addressBookDelegate;
        anchors.fill: parent;
        //上次点击的index
        property int lastIndex: -1;
        //上次点击对应的item
        property var lastItem: null;
        //增加滑动条
        ScrollBar.vertical: ScrollBar
        {
            id: scrollBar;
            visible: false;
            policy: ScrollBar.AsNeeded/*AlwaysOn*/;
        }

        //用户的分类
        //分类是依据名称来的
        section.property: "firstChacter";
        //根据首字母进行过滤
        section.criteria: ViewSection.FullString;
        section.delegate: headDelegate;

    }

    //对象构建完成后，需要设定某一个为选中状态
    Component.onCompleted:
    {
    }

    //列表头部分类委托
    Component
    {
        id:headDelegate;
        Rectangle
        {
            width: parent.width;
            height: 35;
            color: "silver";

            //上方搞一条横线
            Rectangle
            {
                width: parent.width;
                height: 1.5;
                anchors.left: parent.left;
                anchors.top: parent.top;
                color: "black";
            }

            //文字内容区域
            Rectangle
            {
                anchors.left: parent.left;
                anchors.leftMargin: 5;
                anchors.top: parent.top;
                anchors.topMargin: 5;
                color: "transparent";
                Text
                {
                    id: headLeftTitle;
                    text: section;
                    font.family: "msyh";
                    font.pixelSize: 15;
                    anchors.fill:  parent;
                }
            }
        }
    }


    //列表委托
    Component
    {
        id:addressBookDelegate;
        Rectangle
        {
            id: singleItemInView;
            width: parent.width;
            height: 80;

            MouseArea
            {
                anchors.fill: parent;
                //点击这个会话时，上次点击的颜色变成透明，这次点击的变成灰色，并更新这次的index等为上次点击
                onClicked:
                {
                    if(addressBookView.lastItem!==null)
                    {
                        addressBookView.lastItem.color="transparent";
                    }
                    color="grey";
                    addressBookView.currentIndex=index;
                    addressBookView.lastIndex=index;
                    addressBookView.lastItem=addressBookView.currentItem;
                    addressBook.signalFriendListClicked(strIdx,name);
                }
                hoverEnabled: true;
                //鼠标移动到上方时变为银色，如果移动到上次点击的item则不做处理
                onEntered:
                {
                    scrollBar.visible=true;
                    if(addressBookView.currentIndex===index)
                    {
                        return;
                    }
                    color="silver";
                }
                //移走后颜色变为正常
                onExited:
                {
                    scrollBar.visible=false;
                    if(addressBookView.currentIndex===index)
                    {
                        return;
                    }
                    color="transparent";
                }
            }

            //头像区域
            Rectangle
            {
                id:imageLabel;
                color: "#8033CCFF";
                width: 50;
                height: 50;
                radius: 25;
                anchors.top: parent.top;
                anchors.topMargin: 20;
                anchors.left: parent.left;
                anchors.leftMargin: 15;
                Text
                {
                    id: imagePathText;
                    elide: Text.ElideRight;
                    //anchors.fill: parent;
                    font.family: "msyh";
                    font.pixelSize: 14;
                    color: "black";
                    text: model.firstChacter;
                    anchors.centerIn: parent;
                }
            }
            //名字区域
            Rectangle
            {
                id:nameLabel;
                color: "transparent";
                width: 180;
                height: 30;
                anchors.top: parent.top;
                anchors.topMargin: 30;
                anchors.left: imageLabel.right;
                anchors.leftMargin: 15;
                Text
                {
                    id: nameText;
                    elide: Text.ElideRight;
                    anchors.fill: parent;
                    font.family: "msyh";
                    font.pixelSize: 14;
                    color: "black";
                    text: name;
                }
            }

        }
    }

}
