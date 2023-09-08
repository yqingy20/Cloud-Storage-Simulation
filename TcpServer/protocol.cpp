#include "protocol.h"

PDU *mkPDU(uint uiMsglen) {
    uint uiPDULen = sizeof(PDU) + uiMsglen;
    PDU *pdu = (PDU*)malloc(uiPDULen);

    if (pdu == NULL) {
        exit(EXIT_FAILURE);
    }

    memset(pdu, 0, uiPDULen);

    pdu->uiPDULength = uiPDULen;
    pdu->uiMsgLength = uiMsglen;

    return pdu;
}
