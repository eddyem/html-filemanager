var KEY;
const PASSURL="https://ishtar.sao.ru/pass";
function checkcookie(){
	var txt = document.cookie;
	if(txt.length==0 || txt.indexOf('KEY')<0) return 0;
	else return 1;
}
function getcookie(){
	if(!checkcookie())
		document.location.href=PASSURL+"?URL="+document.location.pathname
			+"&RTN="+document.location.href;
}
function onEX(){
	var d = new Date();
	d.setTime(d.getTime() - 1000);
	var str = "KEY=; expires="+d.toGMTString()+"; path="+document.location.pathname;
	document.cookie = str;
	window.location.reload();
}
function exit(){
	var request = new XMLHttpRequest();
	request.open("POST", "https://ishtar.sao.ru/cgi-bin/exit", true);
	request.setRequestHeader("Accept-Charset", "koi8-r");
	request.setRequestHeader("Cookie", document.cookie);
	request.overrideMimeType("multipart/form-data; charset=koi8-r"); 
	request.onreadystatechange=function(){
		if (request.readyState == 4){
			if (request.status == 200){
				onEX();
			}
			else alert("Ошибка соединения");
		}
	}
	request.send("")
}
/*
В sendrequest добавить строчку
	request.setRequestHeader("Cookie", document.cookie);
	
	в код html:
	<button onclick="exit();">Выход</button>
*/
