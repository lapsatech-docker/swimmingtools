#include "swt_file_reader.h"
#include "swt_tests.h"
#include "swt_php.h"
#include "zend_exceptions.h"

static zend_object_handlers swt_swimfile_object_handlers;
static zend_class_entry *php_swt_swimfile_entry;
static zend_class_entry *php_swt_file_not_valid_exception;

struct swimfile_object {
  zend_object std;
  swt::SwimFile *swim_file;
};


PHP_METHOD(swt_SwimFile, __construct)
{
  char *filename;
  int filename_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename,
        &filename_len) == FAILURE) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Must provide a filename");
    RETURN_FALSE;
  }

  swimfile_object *obj  = (swimfile_object*) zend_object_store_get_object(getThis() TSRMLS_CC);
  try {
    swt::FileReader reader;
    std::unique_ptr<swt::SwimFile> swim_file = reader.Read(filename);
    obj->swim_file = swim_file.release();
        
  } catch (swt::FileNotValidException &ex) {
    zend_throw_exception(php_swt_file_not_valid_exception, ex.what(), 0 TSRMLS_CC);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}


PHP_METHOD(swt_SwimFile, changeStroke) {
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;

  long length_index = 0;
  long new_stroke = FIT_SWIM_STROKE_INVALID;
  long stroke_option = swt::kLengthOnly;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &length_index,
        &new_stroke, &stroke_option) == FAILURE) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, 
        "Must provide length index, new stroke, change stroke option");
    RETURN_FALSE;
  }

  switch(new_stroke) {
    case FIT_SWIM_STROKE_BACKSTROKE:
    case FIT_SWIM_STROKE_BREASTSTROKE:
    case FIT_SWIM_STROKE_BUTTERFLY:
    case FIT_SWIM_STROKE_FREESTYLE:
      break;
    default:
      php_error_docref(NULL TSRMLS_CC, E_ERROR, "New stroke not valid");
      RETURN_FALSE;
  }

  switch(stroke_option) {
    case swt::kLengthOnly:
    case swt::kLap:
    case swt::kAll:
      break;
    default:
      php_error_docref(NULL TSRMLS_CC, E_ERROR, "Stroke option not valid");
      RETURN_FALSE;
  }

  try {
    swim_file->ChangeStroke(length_index, new_stroke, 
        static_cast<swt::ChangeStrokeOption>(stroke_option));
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}


PHP_METHOD(swt_SwimFile, changePoolSize) {
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;

  double new_size_metric = 0;
  long display_measure = FIT_DISPLAY_MEASURE_INVALID;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dl", 
        &new_size_metric, &display_measure) == FAILURE) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Must provide new length and display measure");
    RETURN_FALSE;
  }
  
  switch (display_measure) {
    case FIT_DISPLAY_MEASURE_METRIC:
    case FIT_DISPLAY_MEASURE_STATUTE:
      break;
    default:
      php_error_docref(NULL TSRMLS_CC, E_ERROR, "Display measure not valid");
      RETURN_FALSE;
  }

  try {
    swim_file->ChangePoolSize(new_size_metric, display_measure);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}

PHP_METHOD(swt_SwimFile, delete) {
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;

  long length_index = 0;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length_index) == FAILURE) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Must provide length index");
    RETURN_FALSE;
  }

  try {
    swim_file->Delete(length_index);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}

PHP_METHOD(swt_SwimFile, getDateCreated)
{ 
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;
  unsigned long date_created;

  try {
    //FIT define date as seconds since UTC 00:00 Dec 31 1989
    //PHP define date as seconds since UTC 00:00 jan 1 1970
    //Add the difference to date returned by fit
    date_created = swim_file->GetSession()->GetStartTime() + 631065600;
    RETURN_LONG(date_created);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}


PHP_METHOD(swt_SwimFile, getLengths)
{
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;

  array_init(return_value);

  try {
    unsigned long current_lap = 0;
    const std::vector<fit::LapMesg*> &laps = swim_file->GetLaps();
    const std::vector<fit::LengthMesg*> &lengths = swim_file->GetLengths();

    for (fit::LapMesg *lap : laps) {
      if (lap->GetNumActiveLengths() > 0) {
        ++current_lap;
        FIT_UINT16 first_length_index = lap->GetFirstLengthIndex();
        FIT_UINT16 last_length_index = first_length_index + lap->GetNumLengths() - 1;

        for(unsigned int length_index = first_length_index;
            length_index <= last_length_index; ++length_index) {
          fit::LengthMesg *fit_length = lengths.at(length_index);

          if (fit_length->GetLengthType() == FIT_LENGTH_TYPE_ACTIVE) {

            zval *zval_length;
            MAKE_STD_ZVAL(zval_length);
            array_init(zval_length);
            add_assoc_long(zval_length, "lap", current_lap);
            add_assoc_double(zval_length, "time", fit_length->GetTotalTimerTime());
            add_assoc_long(zval_length, "stroke_count", 
                fit_length->GetTotalStrokes());
            add_assoc_long(zval_length, "stroke", fit_length->GetSwimStroke());
            add_assoc_double(zval_length, "rest", swim_file->GetRestTime(length_index));
            std::string error;
            add_assoc_bool(zval_length, "can_edit", 
                swim_file->CanSplitChangeStrokeDelete(fit_length->GetMessageIndex(), &error));
            add_assoc_bool(zval_length, "can_merge", 
                swim_file->CanMerge(fit_length->GetMessageIndex(), &error));
            add_assoc_bool(zval_length, "is_duplicate", 
                swim_file->IsDuplicate(fit_length->GetMessageIndex()));
            add_assoc_long(zval_length, "length_index",
                fit_length->GetMessageIndex());
            add_next_index_zval(return_value, zval_length); 
          }
        }
      }
    }
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
  return;
}

PHP_METHOD(swt_SwimFile, getPoolSize)
{
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;
  double pool_size;

  try {
    pool_size = swim_file->GetSession()->GetPoolLength();
    RETURN_DOUBLE(pool_size);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}

PHP_METHOD(swt_SwimFile, getPoolSizeUnits)
{
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;
  unsigned long pool_size_units;

  try {
    pool_size_units = swim_file->GetSession()->GetPoolLengthUnit();
    RETURN_LONG(pool_size_units);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}

PHP_METHOD(swt_SwimFile, getProduct)
{
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;
  unsigned long product;

  try {
    product = swim_file->GetProduct();
    RETURN_LONG(product);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}

PHP_METHOD(swt_SwimFile, getSerialNumber)
{ 
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;
  unsigned long serial_number;

  try {
    serial_number = swim_file->GetSerialNumber();
    RETURN_LONG(serial_number);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}

PHP_METHOD(swt_SwimFile, getSoftwareVersion)
{ 
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;
  unsigned long software_version;

  try {
    software_version = swim_file->GetSoftwareVersion();
    RETURN_LONG(software_version);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}

PHP_METHOD(swt_SwimFile, merge) {
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;

  long length_index = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length_index) == FAILURE) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Must provide length index");
    RETURN_FALSE;
  }

  try {
    swim_file->Merge(length_index);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}


PHP_METHOD(swt_SwimFile, split) {
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;

  long length_index = 0;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &length_index) == FAILURE) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Must provide length index");
    RETURN_FALSE;
  }

  try {
    swim_file->Split(length_index);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}

PHP_METHOD(swt_SwimFile, save) {
  swimfile_object *obj = (swimfile_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
  swt::SwimFile *swim_file = obj->swim_file;

  char *filename;
  int filename_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename,
        &filename_len) == FAILURE) {
    php_error_docref(NULL TSRMLS_CC, E_ERROR, "Must provide a filename");
    RETURN_FALSE;
  }

  try {
    swim_file->Save(filename);
  } catch (std::exception &ex) {
    zend_throw_exception(zend_exception_get_default(TSRMLS_C), ex.what(), 0 TSRMLS_CC);
  }
}


PHP_FUNCTION(swt_test) {
  swt::Tests tests;
}

static void php_swt_swimfile_dtor(void *object TSRMLS_DC)
{
  swimfile_object *obj = reinterpret_cast<swimfile_object*>(object);
  delete obj->swim_file;

  zend_hash_destroy(obj->std.properties);
  FREE_HASHTABLE(obj->std.properties);

  efree(obj);
}

static zend_object_value php_swt_swimfile_create_handler(zend_class_entry *type TSRMLS_DC) {
  zend_object_value retval;
  swimfile_object * obj = (swimfile_object*) emalloc(sizeof(swimfile_object));
  memset(obj, 0, sizeof(swimfile_object));

  obj->std.ce = type;
  ALLOC_HASHTABLE(obj->std.properties);
  zend_hash_init(obj->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
  object_properties_init(&obj->std, type);

  retval.handle = zend_objects_store_put(obj, NULL, php_swt_swimfile_dtor, NULL TSRMLS_CC);
  retval.handlers = &swt_swimfile_object_handlers;

  return retval;
}

static zend_function_entry php_swt_swimfile_functions[] = {
  PHP_ME(swt_SwimFile, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(swt_SwimFile, changePoolSize, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, changeStroke, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, delete, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, getDateCreated, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, getPoolSizeUnits, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, getLengths, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, getPoolSize, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, getProduct, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, getSerialNumber, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, getSoftwareVersion, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, merge, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, save, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(swt_SwimFile, split, NULL, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL}
};

static zend_function_entry php_swt_functions[] = {
  PHP_FE(swt_test, NULL)
  { NULL, NULL, NULL }
};

PHP_MINIT_FUNCTION(swt)
{
  zend_class_entry ce;
  INIT_NS_CLASS_ENTRY(ce,"swt", "SwimFile", php_swt_swimfile_functions);
  php_swt_swimfile_entry = zend_register_internal_class(&ce TSRMLS_CC);
  php_swt_swimfile_entry->create_object = php_swt_swimfile_create_handler;
  memcpy(&swt_swimfile_object_handlers,
      zend_get_std_object_handlers(), sizeof(zend_object_handlers));

  swt_swimfile_object_handlers.clone_obj = NULL;

  zend_class_entry *exception_ce = zend_exception_get_default(TSRMLS_C);
  INIT_NS_CLASS_ENTRY(ce, "swt", "FileNotValidException",  NULL);
  php_swt_file_not_valid_exception = zend_register_internal_class_ex(&ce, exception_ce, NULL TSRMLS_CC);

  REGISTER_NS_LONG_CONSTANT("swt", "STROKE_BACKSTROKE", FIT_SWIM_STROKE_BACKSTROKE, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "STROKE_BREASTSTROKE", FIT_SWIM_STROKE_BREASTSTROKE, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "STROKE_BUTTERFLY", FIT_SWIM_STROKE_BUTTERFLY, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "STROKE_DRILL", FIT_SWIM_STROKE_DRILL, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "STROKE_FREESTYLE", FIT_SWIM_STROKE_FREESTYLE, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "STROKE_UNKNOWN", FIT_SWIM_STROKE_INVALID, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "STROKE_MIXED", FIT_SWIM_STROKE_MIXED, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "LENGTH_ACTIVE", FIT_LENGTH_TYPE_ACTIVE, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "LENGTH_IDLE", FIT_LENGTH_TYPE_IDLE, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "UNITS_METRIC", FIT_DISPLAY_MEASURE_METRIC, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "UNITS_STATUTE", FIT_DISPLAY_MEASURE_STATUTE, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "CHANGE_STROKE_OPTION_LENGTH_ONLY", swt::kLengthOnly, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "CHANGE_STROKE_OPTION_LAP", swt::kLap, CONST_CS | CONST_PERSISTENT);
  REGISTER_NS_LONG_CONSTANT("swt", "CHANGE_STROKE_OPTION_ALL", swt::kAll, CONST_CS | CONST_PERSISTENT);

  return SUCCESS;
}

zend_module_entry swt_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_SWT_EXTNAME,
  php_swt_functions, /* Functions */
  PHP_MINIT(swt), /* MINIT */
  NULL, /* MSHUTDOWN */
  NULL, /* RINIT */
  NULL, /* RSHUTDOWN */
  NULL, /* MINFO */
#if ZEND_MODULE_API_NO >= 20010901
  PHP_SWT_EXTVER,
#endif
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SWT
ZEND_GET_MODULE(swt)
#endif
