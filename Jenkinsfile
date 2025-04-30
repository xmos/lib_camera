@Library('xmos_jenkins_shared_library@v0.38.0') _

def runningOn(machine) {
  println "Stage running on:"
  println machine
}


getApproval()
pipeline {
  agent {label 'xcore.ai'}

  parameters {
    string(
      name: 'TOOLS_VERSION',
      defaultValue: '15.3.1',
      description: 'The XTC tools version'
    )
    string(
      name: 'XMOSDOC_VERSION',
      defaultValue: 'v7.0.0',
      description: 'The xmosdoc version')

    string(
      name: 'INFR_APPS_VERSION',
      defaultValue: 'develop',
      description: 'The infr_apps version'
    )
  }

  options {
    skipDefaultCheckout()
    timestamps()
    buildDiscarder(xmosDiscardBuildSettings(onlyArtifacts=false))
  } // options

  stages {
    stage('Checkout') {
      steps {
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
      } // steps
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
