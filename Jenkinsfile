node {
    discardOldBuilds maxBuildsToKeep: "25", maxArtifactsToKeep: "10"

    stage "Checkout"
    gitCloneWithSubmodules url: "git@gitlab.com:instil/Surge.git"

    // stage "Core Library"
    // dir("core") {
    //
    // }

    stage "Android Libraries"
    dir("android") {
        try {
            gradleBuild {
                tasks = "clean build"
                archiveFiles = "**/*.aar"
                junitResults = "**/release/TEST-*.xml"
            }
            androidLinter pattern: "**/lint-results*.xml"
        } catch(e) {
            slackNotifyError("Failed to build Android libs, see ${env.BUILD_URL}console")
            error "Build failed"
        }
    }

    // stage "Apple Frameworks"
    // dir("apple") {
    //     try {
    //         sh "./build-release.sh"
    //     } catch(e) {
    //         slackNotifyError("Failed to build Apple frameworks, see ${env.BUILD_URL}console")
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
