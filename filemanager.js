var current_dir_="/Inc";
var request;
var timeout_id;

var utf2koi={
1025:179,
1072:193,
1073:194,
1074:215,
1075:199,
1076:196,
1077:197,
1078:214,
1079:218,
1080:201,
1081:202,
1082:203,
1083:204,
1084:205,
1085:206,
1086:207,
1087:208,
1088:210,
1089:211,
1090:212,
1091:213,
1092:198,
1093:200,
1094:195,
1095:222,
1096:219,
1097:221,
1098:223,
1099:217,
1100:216,
1101:220,
1102:192,
1103:209,
1105:163
};

function $(id){
	return document.getElementById(id);
}

function hexdump(str){
	var ret = "";
	var l = str.length;
	var i, ch, code;
	for(i=0; i<l; i++){
		ch = str.charCodeAt(i);
		if(utf2koi[ch] != null)
			code = utf2koi[ch];
		else if(utf2koi[ch+32] != null)
			code = utf2koi[ch+32]+32;
		else code = ch;
		ch = code.toString(16);
		ret += "%" + ch;
	}
	return ret;
}

function handleError(message) {
	clearTimeout(timeout_id);
	ch_status("Ошибка: "+message)
}

function TimeOut(){
	clearTimeout(timeout_id);
	request.abort();
	handleError("Time over");
}

function sendrequest(req_URL, req_STR, req_fn_INI, req_fn_OK){
	request = new XMLHttpRequest();
	request.open("POST", req_URL, true);
	request.setRequestHeader("Accept-Language", "ru, en");
	request.setRequestHeader("Content-Type", "text/html");
	request.setRequestHeader("Cookie", document.cookie);
	request.onreadystatechange=function(){
		if (request.readyState == 4){
			if (request.status == 200){
				clearTimeout(timeout_id);
				ch_status('');
				req_fn_OK();
			}
			else handleError(request.statusText); // вызвать обработчик ошибки с текстом ответа
		}
	}
	request.send(req_STR);
	ch_status('Обновляю...');
	req_fn_INI();
	timeout_id = setTimeout(TimeOut, 5000);
}

function del_files(){
	var files = document.getElementsByName("file");
	var n = files.length;
	var str_ = "";
	for(i = 0; i < n; i++){
		if(files[i].checked){
			str_ += files[i].id + '%3b';
		}
	}
	function ok(){
		var newFrm = request.responseText;
		var iframe_ = window.dummyframe.document;
		iframe_.open();
		iframe_.write(newFrm);
		iframe_.close();
	};
	function ini(){};
	sendrequest("https://ishtar.sao.ru/cgi-bin/df", str_, ini, ok);
}

function select_all(){
	var files = document.getElementsByName("file");
	var n = files.length;
	for(i = 0; i < n; i++){
		files[i].checked = !files[i].checked;
	}
}

function get_content(dir_){
	$('inout').innerHTML = checkcookie() ? "Выйти" : "Войти";
	if(dir_) current_dir_ = dir_;
	else dir_ = current_dir_;
	function ini(){$('filename').name=dir_;};
	function ok(){$('fmgr_').innerHTML = request.responseText;};
	sendrequest("https://ishtar.sao.ru/cgi-bin/fm", 'DIR=' + dir_, ini, ok);
}

function ch_status(stat){
	$('top').innerHTML = stat;
}

function complete(){
	ch_status("Загружаю файл...");
	document.file__.submit();
	document.file__.reset();
}

function Loaded(str){
	if(!str) str = "";
	ch_status(str);
	get_content();
}

function get_file(file_url){
	window.open(file_url);
}

function make_directory(){
	var curdir = $('filename').name;
	var nd = $('newdir').value;
	var newdir = hexdump(nd);
	var timeout;
	if(!newdir){
		alert("Введите имя директории");
		return;
	}
	function ini(){};
	function ok(){
		var newFrm = request.responseText;
		var iframe_ = window.dummyframe.document;
		iframe_.open();
		iframe_.write(newFrm);
		iframe_.close();
		get_content();
	};
	sendrequest("https://ishtar.sao.ru/cgi-bin/mkdir", 'DIR='+curdir+';NEWDIR='+newdir, ini, ok);
	chdirstat();
}

function chdirstat(){
	var f = $('mknewdir');
	var b = $('mkdirbtn');
	if(f.style.display == 'inline'){
		f.style.display = 'none';
		b.innerHTML = 'Новая директория';
	}
	else{
		f.style.display = 'inline';
		b.innerHTML = 'Отмена';
	}
}

function inout(){
	if(checkcookie()) exit();
	else getcookie();
}