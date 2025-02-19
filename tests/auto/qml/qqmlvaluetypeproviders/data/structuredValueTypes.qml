import QtQml
import Test

MyTypeObject {
    property point p: ({x: 7, y: 77, notthere: 2})
    property size s: ({width: 7, height: "77"})
    property rect r: ({x: 5, y: 55, width: 7, height: 77})

    property point p2
    property size s2
    property rect r2

    property constructible c1: 5
    property constructible c2: ({foo: 7})
    property constructible c3
    property constructible c4

    property list<point> ps: [{x: 1, y: 2}, {x: 3, y: 4}, {x: 55, y: Qt.locale()}]
    property list<size> ss: [{width: 5, height: 6}, {width: 7, height: 8}, {height: 99}]
    property list<constructible> cs: [1, 2, 3, 4, 5, {}]

    property structured b1: ({i: 10, c: 14, p: {x: 1, y: 44} })
    property structured b2
    property list<structured> bb: [{i : 21}, {c: 22}, {p: {x: 199, y: 222}}]

    constructible: 47
    structured: ({i: 11, c: 12, p: {x: 7, y: 8}})

    Component.onCompleted: {
        p2 = {x: 4, y: 5};
        s2 = {width: 7, height: 8};
        r2 = {x: 9, y: 10, width: 11, height: 12};
        c3 = 99;
        b2 = {i: 11, c: 15, p: {x: 4} }

        c4 = {foo: 11};
    }
}
