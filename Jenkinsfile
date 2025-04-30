@Library('xmos_jenkins_shared_library@v0.38.0') _

def runningOn(machine) {
  println "Stage running on:"
  println machine
}

def buildApps(appList) {
  appList.each { app ->
    sh "cmake -G 'Unix Makefiles' -S ${app} -B ${app}/build"
    sh "xmake -C ${app}/build -j\$(nproc)"
  }
}

def checkSkipLink() {
    def skip_linkcheck = ""
    if (env.GH_LABEL_ALL.contains("skip_linkcheck")) {
        println "skip_linkcheck set, skipping link check..."
        skip_linkcheck = "clean html pdf"
    }
    return skip_linkcheck
}

getApproval()
pipeline {
  agent none

  parameters {
      string(
          name: 'TOOLS_VERSION',
          defaultValue: '15.3.1',
          description: 'XTC tools version'
      )
      string(
          name: 'XMOSDOC_VERSION',
          defaultValue: 'v6.3.1',
          description: 'xmosdoc version'
      )
  }

  options {
    skipDefaultCheckout()
    timestamps()
    buildDiscarder(xmosDiscardBuildSettings(onlyArtifacts=false))
  } // options

  stages {
    stage('Checkout') {
      agent {label 'xcore.ai'}
      runningOn(env.NODE_NAME)
      script {
        def (server, user, repo) = extractFromScmUrl()
        env.REPO_NAME = repo
      } // script
      dir(REPO_NAME)
      {
        checkoutScmShallow()
        createVenv(reqFile: "requirements.txt")
      }
    } // Checkout

    stage('Examples build') {
      steps{
         dir("${REPO_NAME}/examples") {
          withVenv { // this repo has Python requirements
            xcoreBuild()
          }
        }
      }
    } // Examples build

    stage('Tests build') {
      steps{
         dir("${REPO_NAME}/tests") {
          withVenv { // this repo has Python requirements
            xcoreBuild()
          }
        }
      }
    } // Tests build


  } // Stages
} // pipeline
