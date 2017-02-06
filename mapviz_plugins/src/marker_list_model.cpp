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
#include <mapviz_plugins/marker_list_model.h>

namespace mapviz_plugins
{
MarkerListModel::MarkerListModel(QObject *parent)
  :
  QAbstractListModel(parent)
{
}

MarkerListModel::~MarkerListModel()
{
}

int MarkerListModel::rowCount(const QModelIndex &index) const
{
  return namespaces_.size();
}

QVariant MarkerListModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || index.parent().isValid()) {
    return QVariant();
  }

  if (index.row() < 0 || static_cast<size_t>(index.row()) >= namespaces_.size()) {
    qWarning("%s called with invalid row (%d >= %zu)",
             __PRETTY_FUNCTION__,
             index.row(),
             namespaces_.size());
    return QVariant();
  }

  int idx = index.row();
  
  if (role == Qt::DisplayRole) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s (%zu markers)",
             namespaces_[idx].name.c_str(),
             namespaces_[idx].markers.size());
    return QString(buffer);
  } else if (role == Qt::CheckStateRole) {
    return namespaces_[idx].visible ? Qt::Checked : Qt::Unchecked;
  }

  return QVariant();
}

/* We have to override flags() to indicate that the items can be
 * checked or unchecked by the user.
 */
Qt::ItemFlags MarkerListModel::flags(const QModelIndex &index) const
{
  return (Qt::ItemIsSelectable |
          Qt::ItemIsDragEnabled |
          Qt::ItemIsDropEnabled |
          Qt::ItemIsUserCheckable |
          Qt::ItemIsEnabled);
}

Qt::DropActions MarkerListModel::supportedDropActions() const
{
  return Qt::MoveAction;
}

/* This setData() implementation allows users to edit the check state
 * of markers in the list.
 */
bool MarkerListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid() || index.parent().isValid()) {
    return false;
  }

  if (index.row() < 0 || static_cast<size_t>(index.row()) >= namespaces_.size()) {
    qWarning("%s called with invalid row (%d >= %zu)",
             __PRETTY_FUNCTION__,
             index.row(),
             namespaces_.size());
    return false;
  }
    
  if (role == Qt::CheckStateRole) {
    int idx = index.row();
    namespaces_[idx].visible = value.toBool();
    return true;
  }

  return false;
}

int MarkerListModel::findNamespace(const std::string &name)
{
  for (size_t i = 0; i < namespaces_.size(); i++) {
    if (namespaces_[i].name == name) {
      return i;
    }
  }

  return -1;
}

void MarkerListModel::addMarker(const visualization_msgs::Marker &marker)
{
  int ns_index = findNamespace(marker.ns);
  if (ns_index < 0) {
    // The namespace was not found so we need to add it.
    ns_index = namespaces_.size();
    beginInsertRows(QModelIndex(), ns_index, ns_index);
    namespaces_.push_back(NamespaceData());
    namespaces_.back().name = marker.ns;
    namespaces_.back().visible = true;
    endInsertRows();
  }  
  NamespaceData &dst = namespaces_[ns_index];

  // Since the item text includes the number of markers, we need to
  // emit a dataChanged signal if the number changes.  We determine if
  // it's a new marker here but emit the signal after the changes have
  // been made.
  bool new_id = dst.markers.count(marker.id) ? true : false;

  dst.markers[marker.id].tmp = 0;

  if (new_id) {
    Q_EMIT dataChanged(index(ns_index), index(ns_index));
  }
}

void MarkerListModel::visibleMarkers(std::vector<MarkerData const *> &markers)
{
  for (size_t i = 0; i < namespaces_.size(); i++) {
    if (!namespaces_[i].visible) {
      continue;
    }

    for (std::map<int, MarkerData>::iterator it = namespaces_[i].markers.begin();
         it != namespaces_[i].markers.end();
         ++it) {
      markers.push_back(&it->second);
    }
  }
}
}  // namespace mapviz_plugins
