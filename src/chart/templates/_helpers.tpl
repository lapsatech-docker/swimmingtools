{{- define "func.string.appHost" -}}
  {{- $.Values.app.host -}} 
{{- end -}}

{{- define "func.string.appPort" -}}
  {{- $.Values.app.port -}} 
{{- end -}}

{{- define "func.string.nginxPort" -}}
  {{- $.Values.nginx.port -}} 
{{- end -}}

{{- define "func.string.phpPort" -}}
  {{- $.Values.php.port -}} 
{{- end -}}

{{- define "func.string.imageTag" -}}
  {{- $.Values.image.tag -}}
{{- end -}}
