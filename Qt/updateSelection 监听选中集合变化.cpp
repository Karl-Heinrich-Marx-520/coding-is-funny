void updateSelection(const QItemSelection& selected, const QItemSelection& deselected);

void MainWindow::updateSelection(const QItemSelection& selected, const QItemSelection& deselected){
    QModelIndex index;
    QModelIndexList list = selected.indexes();
    //为现在的选择项目设置值
    for(int i =0; i < list.size(); i++){
        QString text = QString("(%1, %2)").arg(list[i].row()).arg(list[i].column());
        _table_view->model()->setData(list[i], text);
    }

    list = deselected.indexes();
    foreach(index, list){
        _table_view->model()->setData(index,"");
    }
}

connect(selection_model, &QItemSelectionModel::selectionChanged, this, &MainWindow::updateSelection);
