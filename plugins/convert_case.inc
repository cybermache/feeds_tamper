<?php

/**
 * @file
 * Convert text to upper, lower, or title case.
 */

$plugin = array(
  'form' => 'feeds_tamper_convert_case_form',
  'callback' => 'feeds_tamper_convert_case_callback',
  'name' => 'Convert case',
  'multi' => 'loop',
  'category' => 'Text',
);

function feeds_tamper_convert_case_form($importer, $element_key, $settings) {
  $form = array();
  $form['mode'] = array(
    '#type' => 'radios',
    '#title' => t('How to convert case'),
    '#default_value' => isset($settings['mode']) ? $settings['mode'] : MB_CASE_TITLE,
    '#options' => array(
      MB_CASE_TITLE => t('Title Case'),
      MB_CASE_LOWER => t('lower case'),
      MB_CASE_UPPER => t('UPPER CASE'),
      'ucfirst' => t('Sentence case'),
    ),
  );
  $form['exclude'] = array(
    '#type' => 'textfield',
    '#title' => t('Words to exclude'),
    '#states' => array(
      'visible' => array(
        ':input[name="settings[mode]"]' => array(
          array('value' => MB_CASE_TITLE),
          array('value' => 'ucfirst'),
        ),
      ),
    ),    
    '#default_value' => isset($settings['exclude']) ? $settings['exclude'] : 'to,a,the,of,by,and,with',
    '#description' => t('A comma separated list of words that should excluded form conversion. You can include words that are all capitals such as \'BBQ\' to avoid being alter to \'Bbq\'. Leaving blank will include all words.'),
  );
  return $form;
}

function feeds_tamper_convert_case_callback($result, $item_key, $element_key, &$field, $settings, $source) {
  if ($settings['mode'] === 'ucfirst') {
    $field = mb_strtoupper(mb_substr($field, 0, 1, 'UTF-8'), 'UTF-8') . mb_strtolower(mb_substr($field, 1, mb_strlen($field, 'UTF-8'), 'UTF-8'));
  }
  elseif ($settings['mode'] == MB_CASE_TITLE) {
    if (!empty($settings['exclude'])) {
      $delimiters = array(" ", "-", "\n");
      $exceptions = explode(',',$settings['exclude']);
      /* 
       * Exceptions in lower case are words you don't want converted 
       * Exceptions all in upper case are any words you don't want converted to title case 
       *   but should be converted to upper case, e.g.: 
       *   bbq or Bbq should be BBQ
       */ 
      foreach ($delimiters as $delimiter) { 
        $words = explode($delimiter, $field); 
        $newwords = array(); 
        foreach ($words as $word) { 
          if (in_array(strtoupper($word), $exceptions)) { 
            // check exceptions list for any words that should be in upper case 
            $word = strtoupper($word); 
          } elseif (!in_array($word, $exceptions)) { 
            // convert to uppercase 
            $word = ucfirst($word); 
          } 
          array_push($newwords, $word); 
        } 
        $field = join($delimiter, $newwords); 
      }
      return $field;
    }
    else {
      $field = mb_convert_case($field, $settings['mode'], 'UTF-8');
    }
  }
  else {
    $field = mb_convert_case($field, $settings['mode'], 'UTF-8');
  }
}
