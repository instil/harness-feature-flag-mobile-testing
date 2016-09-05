properties([
    buildDiscarder(logRotator(numToKeepStr: '25', artifactNumToKeepStr: '10')),
    pipelineTriggers([[$class: "SCMTrigger", scmpoll_spec: "H/15 * * * *"]])
])

node {
    stage "Checkout"
    gitCloneWithSubmodules url: "git@gitlab.com:instil/Surge.git"

    stage "Build Apple Frameworks"
    dir("apple") {
        try {
            sh "./build-release.sh"
            publishJunitTestResults "build/test-results/*.xml"
            archive includes: "build/frameworks/apple-frameworks.zip"
        } catch(e) {
            slackNotifyError("Failed to build Apple frameworks, see ${env.BUILD_URL}console")
            error "Build failed"
        }
    }

    stage "Build Android Libraries"
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
}

def slackNotify(message) {
    slackSend channel: "#surge", color: "good", message: message
}

def slackNotifyError(message) {
    slackSend channel: "#surge", color: "danger", message: message
}
