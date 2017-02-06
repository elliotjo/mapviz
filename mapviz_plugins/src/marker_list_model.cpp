// *****************************************************************************
//
// Copyright (c) 2017, Southwest Research Institute® (SwRI®)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Southwest Research Institute® (SwRI®) nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *****************************************************************************
#include "marker_list_model.h"

namespace mapviz_plugins
{
MarkerListModel::MarkerListModel(QObject *parent)
  :
  QAbstractListModel(parent),
  direction_(true)
{
  startTimer(500);
}

MarkerListModel::~MarkerListModel()
{
}

int MarkerListModel::rowCount(const QModelIndex &index) const
{
  return tmp_.size();
}

QVariant MarkerListModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  if (role == Qt::DisplayRole) {
    if (index.row() < tmp_.size()) {
      return QString::fromStdString(tmp_[index.row()]);
    } else {
      return QVariant("Invalid index!");
    }
  }

  return QVariant();
}

void MarkerListModel::timerEvent(QTimerEvent *event)
{
  if (tmp_.size() >= 20 && direction_) {
    direction_ = false;
  } else if (tmp_.size() == 0 && !direction_) {
    direction_ = true;
  }
  
  if (direction_) {
    beginInsertRows(QModelIndex(), tmp_.size(), tmp_.size());
    tmp_.push_back("testing");
    endInsertRows();
  } else {
    beginRemoveRows(QModelIndex(), tmp_.size()-1, tmp_.size()-1);
    tmp_.pop_back();
    endRemoveRows();
  } 
}
}  // namespace mapviz_plugins
