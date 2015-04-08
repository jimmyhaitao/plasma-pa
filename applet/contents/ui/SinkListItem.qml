import QtQuick 2.0

import org.kde.plasma.private.volume 0.1

DeviceListItemBase {
//    icon: 'audio-card'
    subModel: ReverseFilterModel {
        sortRole: SinkInputModel.IndexRole;
        filterRole: SinkInputModel.SinkIndexRole;
        filterRegExp: new RegExp(Index);
        sourceModel: SinkInputModel {
            context: pulseContext
        }

        Component.onCompleted: initialSort();
    }
    subDelegate: SinkInputListItem {}

    function setVolume(volume) {
        // TODO: possibly set the volume icon here, otherwise it will update on delay
        pulseContext.setSinkVolume(Index, volume);
    }
}
