node {
    discardOldBuilds maxBuildsToKeep: "25", maxArtifactsToKeep: "10"

    stage "Checkout"
    gitCloneWithSubmodules url: "git@gitlab.com:instil/Surge.git"

    stage "Android"
    dir("android") {
        try {
            gradleBuild {
                tasks = "clean build"
                archiveFiles = "**/*.aar"
                junitResults = "**/release/TEST-*.xml"
            }
        } catch(e) {
            slackNotifyError("Build #${env.BUILD_NUMBER} failed to build Android libs, see ${env.BUILD_URL}console")
            error "Build failed"
        }
    }

    // stage "Apple"
    // dir("apple") {
    //     try {
    //         sh "mkdir build"
    //         dir("build") {
    //             sh "cmake -DFOR_IOS=ON -G Xcode ../../core"
    //         }
    //
    //     } catch(e) {
    //         // slackNotifyError("Build #${env.BUILD_NUMBER} failed to build Apple frameworks, see ${env.BUILD_URL}console")
    //         error "Build failed"
    //     }
    // }
}

def slackNotify(message) {
    slackSend channel: "#surge", color: "good", message: message
}

def slackNotifyError(message) {
    slackSend channel: "#surge", color: "danger", message: message
}
