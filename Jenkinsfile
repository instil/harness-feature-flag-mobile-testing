properties([
    gitLabConnection("Instil GitLab"),
    buildDiscarder(logRotator(numToKeepStr: "25", artifactNumToKeepStr: "10"))
])

node("ios && android") {
    stage("Checkout") {
        checkout scm
    }

    gitlabCommitStatus {
        stage("Build Apple Frameworks") {
            buildAppleFrameworks()
        }

        stage("Build Android Libraries") {
            buildAndroidLibraries()
        }

        stage("Build Xamarin DLLs") {
            buildXamarinDlls()
        }
    }
}

def buildAppleFrameworks() {
    try {
        dir("apple") {
            sh "./build-release.sh"
            // junit "build/test-results/*.xml"
            archiveArtifacts "build/frameworks/apple-frameworks.zip"
        }
    } catch(e) {
        slackNotifyError("Failed to build Apple frameworks, see ${env.BUILD_URL}console")
        error "Build failed"
    }
}

def buildAndroidLibraries() {
    try {
        installAndroidSdkPackages()

        dir("android") {
            sh "./gradlew clean build"
            junit "**/testReleaseUnitTest/TEST-*.xml"
            archiveArtifacts "**/*.aar"
            androidLint pattern: "**/lint-results*.xml"
        }
    } catch(e) {
        slackNotifyError("Failed to build Android libs, see ${env.BUILD_URL}console")
        error "Build failed"
    }
}

def buildXamarinDlls() {
    try {
        dir("xamarin") {
            dir("SurgeXamarinAndroid") {
                sh "mkdir -p SurgeXamarinAndroidBindings/Jars"
                sh "cp ../../android/surge/build/outputs/aar/surge-release.aar SurgeXamarinAndroidBindings/Jars/SurgeAndroid.aar"
                sh "/Library/Frameworks/Mono.framework/Versions/Current/Commands/msbuild /p:Configuration=Release SurgeXamarinAndroid.sln"
                archiveArtifacts "SurgeXamarinAndroidBindings/bin/Release/*.dll"
            }
            dir("SurgeXamariniOS") {
                sh "mkdir lib"
                sh "cp -r ../../apple/build/frameworks/iOS/SurgeiOS.framework lib"
                sh "cp ../../apple/build/frameworks/iOS/libSurgeCore.a lib"
                sh "/Library/Frameworks/Mono.framework/Versions/Current/Commands/msbuild /t:Rebuild /p:Configuration=Release SurgeXamariniOS.sln"
                archiveArtifacts "SurgeXamariniOSBindings/bin/Release/*.dll"
            }
        }
    } catch(e) {
        slackNotifyError("Failed to build Xamarin DLLs, see ${env.BUILD_URL}console")
        error "Build failed"
    }
}

def slackNotify(message) {
    slackSend channel: "#surge", color: "good", message: message
}

def slackNotifyError(message) {
    slackSend channel: "#surge", color: "danger", message: message
}
