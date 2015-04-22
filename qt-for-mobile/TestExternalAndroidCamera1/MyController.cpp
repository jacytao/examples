#include "MyController.h"

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

#include <QDebug>

void MyController::callExternalCamera()
{
#ifdef Q_OS_ANDROID
	//android.provider.MediaStore.EXTRA_OUTPUT
	QAndroidJniObject MediaStore__EXTRA_OUTPUT
			= QAndroidJniObject::getStaticObjectField(
				"android/provider/MediaStore", "EXTRA_OUTPUT", "Ljava/lang/String;");
	qDebug() << "MediaStore__EXTRA_OUTPUT.isValid()=" << MediaStore__EXTRA_OUTPUT.isValid();


	QAndroidJniObject action = QAndroidJniObject::fromString("android.media.action.IMAGE_CAPTURE");
	QAndroidJniObject intent=QAndroidJniObject("android/content/Intent","(Ljava/lang/String;)V",
											   action.object<jstring>());
	qDebug() << __FUNCTION__ << "intent.isValid()=" << intent.isValid();

	QAndroidJniObject extDir = QAndroidJniObject::callStaticObjectMethod(
				"android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
	qDebug() << "extDir.isValid()=" << extDir.isValid();

	QAndroidJniObject filename = QAndroidJniObject::fromString("camera.jpg");

	QAndroidJniObject photo=QAndroidJniObject("java/io/File","(Ljava/io/File;Ljava/lang/String;)V",
											   extDir.object<jobject>(), filename.object<jstring>());
	qDebug() << __FUNCTION__ << "photo.isValid()=" << photo.isValid();

	takePhotoSavedUri = QAndroidJniObject::callStaticObjectMethod(
				"android/net/Uri", "fromFile", "(Ljava/io/File;)Landroid/net/Uri;", photo.object<jobject>());
	qDebug() << "takePhotoSavedUri.isValid()=" << takePhotoSavedUri.isValid();

	intent.callObjectMethod(
				"putExtra","(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;",
				MediaStore__EXTRA_OUTPUT.object<jstring>(), takePhotoSavedUri.object<jobject>());
	qDebug() << __FUNCTION__ << "intent.isValid()=" << intent.isValid();

	int SHOOT_PHOTO = 1;
	QtAndroid::startActivity(intent, SHOOT_PHOTO, this);
#endif

}

#ifdef Q_OS_ANDROID
void MyController::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject & data)
{
	int SHOOT_PHOTO = 1;
	jint Activity__RESULT_OK = QAndroidJniObject::getStaticField<jint>(
				"android.app.Activity", "RESULT_OK");

	if ( receiverRequestCode == SHOOT_PHOTO && resultCode == Activity__RESULT_OK )
	{
		qDebug() << "takePhotoSavedUri:" << takePhotoSavedUri.toString();

		QAndroidJniObject absPath = takePhotoSavedUri.callObjectMethod("getPath","()Ljava/lang/String;");
		qDebug() << __FUNCTION__ << "absPath.isValid()=" << absPath.isValid();

		m_imagePath = absPath.toString();
		emit this->imagePathChanged();
	}
}
#endif
