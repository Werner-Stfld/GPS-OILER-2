import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('wifi')

    function apply () {
      console.log("apply wifi settings: " + JSON.stringify(settings))
    }
    return {
      apply, settings
    }
  },
  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>WiFi</legend>\
        <labelled-input summary="SSID" desc="Identifier des WiFi-Netzwerks."/>\
        <input id="SSID" type="text" v-model="settings.name" name="name"/>\
        <labelled-input summary="Passwort" desc="Das Passwort zum Absichern des WiFi Zugriffs. Das Passwort sollte mindestens 8 Zeichen lang sein. Ein leeres Passwort erlaubt uneingeschränkten Zugriff."/>\
        <input id="Passwort" type="text" v-model="settings.password" name="password" />\
        <labelled-input summary="Timeout" desc="Der Timeout in Minuten legt fest, wann das Protokoll abgeschaltet wird. Die Bedienung der WEB Schnittstelle verlängert den Timeout."/>\
        <input id="Timeout" type="number" min='1' inputmode='dec' v-model="settings.timeout" name="timeout"/>\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}