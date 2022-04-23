import QtQuick 2.3

Rectangle {
    width: 200
    height: 100
    color: "transparent"

    //同样是一个listview
    ListView
    {
        id:addFriendListView;
        model: addFriendListModel;
        delegate: addFriendListDelegate;
    }

    ListModel
    {
        id:addFriendListModel;
    }

    Component
    {
        id:addFriendListDelegate;
        Rectangle
        {
            width: 100;
            height: 100;
        }
    }

}
