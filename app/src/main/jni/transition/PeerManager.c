//
// Created by Mihail Gutan on 12/11/15.
//
#include "PeerManager.h"
#include "BRPeerManager.h"
#include "BRPeer.h"
#include "WalletCallbacks.h"
#include "BRInt.h"
#include <android/log.h>
#include "BRMerkleBlock.h"
#include "BRWallet.h"
#include "wallet.h"

static BRMerkleBlock **_blocks;
static JavaVM *_jvm;
static BRPeer *_peers;
static size_t _blocksCounter = 0;
static size_t _peersCounter = 0;


static jobject getPeerManagerInstance() {
    JNIEnv *env;
    jint rs = (*_jvm)->AttachCurrentThread(_jvm, &env, NULL);

    jclass clazz = (*env)->FindClass(env, "com/breadwallet/wallet/BRPeerManager");
    jfieldID instanceFid = (*env)->GetStaticFieldID(env, clazz, "instance",
                                                    "Lcom/breadwallet/wallet/BRPeerManager;");

    jobject instance;
    if (instanceFid == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "Message from C: ",
                            "instanceFid is null!!!! returning ");
        return NULL;
    }
    instance = (*env)->GetStaticObjectField(env, clazz, instanceFid);
    if (instance == NULL) {
        instance = (*env)->AllocObject(env, clazz);
        (*env)->SetObjectField(env, clazz, instanceFid, instance);
    }

    return instance;
}

static void syncStarted(void *info) {
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "syncStarted: info - %s",info );
    JNIEnv *globalEnv;
    jint rs = (*_jvm)->AttachCurrentThread(_jvm, &globalEnv, NULL);
    jobject instance = getPeerManagerInstance();
    jclass clazz = (*globalEnv)->FindClass(globalEnv, "com/breadwallet/wallet/BRPeerManager");
    jmethodID mid = (*globalEnv)->GetMethodID(globalEnv, clazz, "syncStarted", "()V");
//    uint64_t walletBalance = BRWalletBalance(wallet);
//    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ",
//                        "BRWalletBalance(wallet): %d", BRWalletBalance(wallet));
    //call java methods
    (*globalEnv)->CallVoidMethod(globalEnv, instance, mid);

}

static void syncSucceeded(void *info) {
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "syncSucceeded: info - %s",info );
    JNIEnv *globalEnv;
    jint rs = (*_jvm)->AttachCurrentThread(_jvm, &globalEnv, NULL);

}

static void syncFailed(void *info, int error) {
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "syncFailed: info - %s",info );
    JNIEnv *globalEnv;
    jint rs = (*_jvm)->AttachCurrentThread(_jvm, &globalEnv, NULL);

}

static void txStatusUpdate(void *info) {
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "txStatusUpdate: info - %s",info );
    JNIEnv *globalEnv;
    jint rs = (*_jvm)->AttachCurrentThread(_jvm, &globalEnv, NULL);

}

static void txRejected(void *info, int rescanRecommended) {
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ",
                        "txRejected: info - %s , rescanRecommended - %d", info, rescanRecommended );
    JNIEnv *globalEnv;
    jint rs = (*_jvm)->AttachCurrentThread(_jvm, &globalEnv, NULL);

}

static void saveBlocks(void *info, BRMerkleBlock *blocks[], size_t count) {
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "saveBlocks");

    JNIEnv *globalEnv;
    jint rs = (*_jvm)->AttachCurrentThread(_jvm, &globalEnv, NULL);

    //create class
    jclass clazz = (*globalEnv)->FindClass(globalEnv, "com/breadwallet/wallet/BRPeerManager");
    jobject entity = (*globalEnv)->AllocObject(globalEnv, clazz);
    jmethodID mid = (*globalEnv)->GetMethodID(globalEnv, clazz, "saveBlocks", "([B)V");
    //call java methods

    for (int i = 0; i < count; i++) {
        uint8_t buf[BRMerkleBlockSerialize(blocks[i], NULL, 0)];
        size_t len = BRMerkleBlockSerialize(blocks[i], buf, sizeof(buf));
        jbyteArray result = (*globalEnv)->NewByteArray(globalEnv, len);
        (*globalEnv)->SetByteArrayRegion(globalEnv, result, 0, len, (jbyte *) buf);
        (*globalEnv)->CallVoidMethod(globalEnv, entity, mid, result);
    }

}

static void savePeers(void *info, const BRPeer peers[], size_t count) {
    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "savePeers");
    JNIEnv *globalEnv;
    jint rs = (*_jvm)->AttachCurrentThread(_jvm, &globalEnv, NULL);

    //create class
    jclass clazz = (*globalEnv)->FindClass(globalEnv, "com/breadwallet/wallet/BRPeerManager");
    jobject entity = (*globalEnv)->AllocObject(globalEnv, clazz);
    jmethodID mid = (*globalEnv)->GetMethodID(globalEnv, clazz, "savePeers", "([B[B[B)V");
    //call java methods

    for (int i = 0; i < count; i++) {

        jbyteArray peerAddress = (*globalEnv)->NewByteArray(globalEnv, sizeof(peers[i].address));
        (*globalEnv)->SetByteArrayRegion(globalEnv, peerAddress, 0, sizeof(peers[i].address), (jbyte *) &peers[i].address);

        jbyteArray peerPort = (*globalEnv)->NewByteArray(globalEnv, sizeof(peers[i].port));
        (*globalEnv)->SetByteArrayRegion(globalEnv, peerPort, 0, sizeof(peers[i].port), (jbyte *) &peers[i].port);

        jbyteArray peerTimeStamp = (*globalEnv)->NewByteArray(globalEnv, sizeof(peers[i].timestamp));
        (*globalEnv)->SetByteArrayRegion(globalEnv, peerTimeStamp, 0, sizeof(peers[i].timestamp), (jbyte *) &peers[i].timestamp);

        (*globalEnv)->CallVoidMethod(globalEnv, entity, mid, peerAddress, peerPort, peerTimeStamp);
    }
}

static int networkIsReachable(void *info) {
//    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "networkIsReachable");
}

JNIEXPORT void Java_com_breadwallet_wallet_BRPeerManager_connect(JNIEnv *env, jobject thiz,
                                                                 jlong earliestKeyTime,
                                                                 jlong blocksCount,
                                                                 jlong peersCount) {
    jint rs = (*env)->GetJavaVM(env, &_jvm);
    if (rs != JNI_OK){
        __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "WARNING, GetJavaVM is not JNI_OK");
    }

    if (_wallet == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "NULL: %s", "blocks");
        return;
    }

    BRPeerManager *peerManager = BRPeerManagerNew(_wallet, earliestKeyTime,
                                                  blocksCount == 0 ? NULL : _blocks,
                                                  blocksCount, peersCount == 0 ? NULL : _peers,
                                                  peersCount);
    if (peerManager == NULL) {
        __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "NULL: ", "peerManager");
        return;
    }

    BRPeerManagerSetCallbacks(peerManager, NULL, syncStarted, syncSucceeded, syncFailed,
                              txStatusUpdate, txRejected, saveBlocks, savePeers,
                              networkIsReachable);
    BRPeerManagerConnect(peerManager);
}

//Call multiple times with all the blocks from the DB
JNIEXPORT void Java_com_breadwallet_wallet_BRPeerManager_putBlock(JNIEnv *env,
                                                                  jobject thiz,
                                                                  jbyteArray block) {
    int bkLength = (*env)->GetArrayLength(env, block);
    jbyte *byteBk = (*env)->GetByteArrayElements(env, block, 0);
    BRMerkleBlock *tmpBk = BRMerkleBlockParse(byteBk, bkLength);
//    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "adding a block: blockhight: %d, "
//            "transactionCounter: %d", tmpTx->blockHeight, _transactionsCounter);
    _blocks[_blocksCounter++] = tmpBk;
}

JNIEXPORT void Java_com_breadwallet_wallet_BRPeerManager_createBlockArrayWithCount(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   size_t bkCount) {
    _blocks = calloc(bkCount, sizeof(BRMerkleBlock));
    // need to call free();
}

//Call multiple times with all the peers from the DB
JNIEXPORT void Java_com_breadwallet_wallet_BRPeerManager_putPeer(JNIEnv *env,
                                                                 jobject thiz,
                                                                 jbyteArray peerAddress,
                                                                 jbyteArray peerPort,
                                                                 jbyteArray peerTimeStamp) {
    int addrLength = (*env)->GetArrayLength(env, peerAddress);
    jbyte *byteAddr = (*env)->GetByteArrayElements(env, peerAddress, 0);

    int portLength = (*env)->GetArrayLength(env, peerPort);
    jbyte *bytePort = (*env)->GetByteArrayElements(env, peerPort, 0);

    int stampLength = (*env)->GetArrayLength(env, peerTimeStamp);
    jbyte *byteStamp = (*env)->GetByteArrayElements(env, peerTimeStamp, 0);

    BRPeer tmpPr;
    tmpPr.address = *(UInt128 *) byteAddr;
    tmpPr.port = *(uint16_t *) bytePort;
    tmpPr.timestamp = *(uint64_t *) byteStamp;
    tmpPr.services = SERVICES_NODE_NETWORK;
    tmpPr.flags = 0;

    _peers[_peersCounter++] = tmpPr;

//    (jbyte *)&tmpPr.address;
//    sizeof(tmpPr.address);

//            BRMerkleBlockParse(byteBk, bkLength);
//    __android_log_print(ANDROID_LOG_ERROR, "Message from C: ", "adding a block: blockhight: %d, "
//            "transactionCounter: %d", tmpTx->blockHeight, _transactionsCounter);

}

JNIEXPORT void Java_com_breadwallet_wallet_BRPeerManager_createPeerArrayWithCount(JNIEnv *env,
                                                                                  jobject thiz,
                                                                                  size_t prCount) {
    _peers = calloc(prCount, sizeof(BRPeer));
    // need to call free();
}