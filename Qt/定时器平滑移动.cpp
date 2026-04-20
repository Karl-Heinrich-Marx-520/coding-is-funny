// 1. 在头文件添加成员变量
// private:
//     QTimer *timer;
//     bool keyUp,keyDown,keyLeft,keyRight;

// 2. 构造函数初始化定时器
timer = new QTimer(this);
connect(timer, &QTimer::timeout, this, [=](){
    QPoint p = ui->pushButton->pos();
    if(keyUp) p.setY(p.y()-5);
    if(keyDown) p.setY(p.y()+5);
    if(keyLeft) p.setX(p.x()-5);
    if(keyRight) p.setX(p.x()+5);
    ui->pushButton->move(p);
});
timer->start(20); // 20ms刷新一次，超流畅

// 3. 按下按键
void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat()) return;
    switch(event->key()){
        case Qt::Key_Up:    keyUp = true; break;
        case Qt::Key_Down:  keyDown = true; break;
        case Qt::Key_Left:  keyLeft = true; break;
        case Qt::Key_Right: keyRight = true; break;
    }
}

// 4. 松开按键
void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat()) return;
    switch(event->key()){
        case Qt::Key_Up:    keyUp = false; break;
        case Qt::Key_Down:  keyDown = false; break;
        case Qt::Key_Left:  keyLeft = false; break;
        case Qt::Key_Right: keyRight = false; break;
    }
}
