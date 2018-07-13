
import QtQuick 2.7

import com.voidware.brahman 1.0

FontLoader
{
    onStatusChanged: if (status == FontLoader.Ready) 
    console.log("loaded font", name)
}
